#include <mmo/infrastructure/Net/NetServer.hpp>
#include <mmo/infrastructure/Net/PacketCodec.hpp>
#include <mmo/infrastructure/Net/TcpConnection.hpp>

// 🌟 引入已冻结的基础设施契约
#include <mmo/infrastructure/lockfree_queue/MPSCQueue.hpp>

#include <boost/asio.hpp>
#include <thread>
#include <mutex>
#include <unordered_map>
#include <array>
#include <optional>

namespace mmo::infrastructure::net
{
    using boost::asio::ip::tcp;

    struct NetServer::Impl
    {
        boost::asio::io_context ioContext;

        using WorkGuard = boost::asio::executor_work_guard<boost::asio::io_context::executor_type>;
        std::optional<WorkGuard> workGuard;

        std::unique_ptr<tcp::acceptor> acceptor;
        std::thread ioThread;

        std::atomic<ConnectionId> nextConnectionId{1};

        std::unordered_map<ConnectionId, std::shared_ptr<TcpConnection>> connections;
        std::mutex connectionMutex;

        // 🌟 完美对齐契约点 1：定义编译期定长容量，契约完全合一
        static constexpr std::size_t kInboundQueueCapacity = 65536;
        mmo::infrastructure::lockfree_queue::MPSCQueue<ReceivedPacket, kInboundQueueCapacity> inboundQueue;

        void StartAccept();
        void StartReadLoop(std::shared_ptr<TcpConnection> connection);
        void StartWriteLoop(std::shared_ptr<TcpConnection> connection);

        // 🌟 完美对齐契约点 4：统一收拢生命周期回收
        void RemoveConnection(ConnectionId id)
        {
            std::scoped_lock lock(connectionMutex);
            connections.erase(id);
        }
    };

    NetServer::NetServer() : impl_(std::make_unique<Impl>()) {}

    NetServer::~NetServer() { Stop(); }

    bool NetServer::Start(std::uint16_t port)
    {
        if (running_)
            return false;
        try
        {
            impl_->workGuard.emplace(boost::asio::make_work_guard(impl_->ioContext));
            impl_->acceptor = std::make_unique<tcp::acceptor>(impl_->ioContext, tcp::endpoint(tcp::v4(), port));
            impl_->StartAccept();
            impl_->ioThread = std::thread([this]()
                                          { impl_->ioContext.run(); });
            running_ = true;
            return true;
        }
        catch (...)
        {
            impl_->workGuard.reset();
            return false;
        }
    }

    void NetServer::Stop()
    {
        if (!running_)
            return;
        running_ = false;

        impl_->workGuard.reset();

        if (impl_->acceptor && impl_->acceptor->is_open())
        {
            boost::system::error_code ec;
            impl_->acceptor->close(ec);
        }

        impl_->ioContext.stop();
        if (impl_->ioThread.joinable())
        {
            impl_->ioThread.join();
        }

        {
            std::scoped_lock lock(impl_->connectionMutex);
            for (auto &[id, conn] : impl_->connections)
            {
                conn->Disconnect();
            }
            impl_->connections.clear();
        }

        // 🌟 完美对齐契约点 2：使用已冻结的标准契约方法 Dequeue() 进行排空清理
        ReceivedPacket temp;
        while (impl_->inboundQueue.Dequeue(temp))
        {
            // Drop & Purge remaining objects memory
        }
    }

    bool NetServer::IsRunning() const noexcept { return running_; }

    bool NetServer::Send(const OutboundPacket &packet)
    {
        std::shared_ptr<TcpConnection> conn;
        {
            std::scoped_lock lock(impl_->connectionMutex);
            auto it = impl_->connections.find(packet.connectionId);
            if (it == impl_->connections.end())
                return false;
            conn = it->second;
        }

        auto bytes = PacketCodec::Encode(packet.packetId, packet.payload);

        // 如果触发背压线，EnqueueSend 返回 false，直接拒绝投递并向上层暴露
        if (!conn->EnqueueSend(std::move(bytes)))
        {
            // 🌟 技术债 NET-002 提示点：在此可以断开该过载连接
            return false;
        }

        impl_->StartWriteLoop(conn);
        return true;
    }

    bool NetServer::PollPacket(ReceivedPacket &packet)
    {
        // 🌟 完美对齐契约点 3：消除自定义的 TryPop，改回无锁队列标准的 Dequeue()
        return impl_->inboundQueue.Dequeue(packet);
    }

    std::size_t NetServer::ConnectionCount() const noexcept
    {
        std::scoped_lock lock(impl_->connectionMutex);
        return impl_->connections.size();
    }

    void NetServer::Impl::StartAccept()
    {
        auto socket = std::make_shared<tcp::socket>(ioContext);
        acceptor->async_accept(*socket, [this, socket](boost::system::error_code ec)
                               {
            if (ec == boost::asio::error::operation_aborted) return;
            if (!ec)
            {
                auto id = nextConnectionId++;
                auto connection = std::make_shared<TcpConnection>(id, std::move(*socket));
                {
                    std::scoped_lock lock(connectionMutex);
                    connections.emplace(id, connection);
                }
                StartReadLoop(connection);
            }
            if (acceptor && acceptor->is_open()) StartAccept(); });
    }

    void NetServer::Impl::StartReadLoop(std::shared_ptr<TcpConnection> connection)
    {
        auto tempChunk = std::make_shared<std::array<std::uint8_t, 4096>>();
        connection->GetSocket().async_read_some(
            boost::asio::buffer(*tempChunk),
            [this, connection, tempChunk](boost::system::error_code ec, std::size_t bytesTransferred)
            {
                if (ec)
                {
                    connection->Disconnect();
                    RemoveConnection(connection->GetId()); // 🌟 统一收拢
                    return;
                }

                auto &streamBuffer = connection->ReceiveBuffer();
                streamBuffer.insert(streamBuffer.end(), tempChunk->begin(), tempChunk->begin() + bytesTransferred);

                while (true)
                {
                    ReceivedPacket packet;
                    if (!PacketCodec::Decode(streamBuffer, packet))
                        break;
                    packet.connectionId = connection->GetId();

                    // 🌟 完美对齐契约点 2：改用标准契约 Enqueue 方法，若满则实行丢包策略
                    if (!inboundQueue.Enqueue(std::move(packet)))
                    {
                        // TODO: 未来演进为对接 Layer1/Metrics 的丢包统计挂钩
                    }
                }

                StartReadLoop(connection);
            });
    }

    void NetServer::Impl::StartWriteLoop(std::shared_ptr<TcpConnection> connection)
    {
        if (!connection->TryBeginWrite())
        {
            return;
        }

        ByteBuffer packet;
        if (!connection->TryPopSend(packet))
        {
            connection->EndWrite();
            if (connection->HasPendingSend())
            {
                StartWriteLoop(connection);
            }
            return;
        }

        auto sendBuffer = std::make_shared<ByteBuffer>(std::move(packet));

        boost::asio::async_write(
            connection->GetSocket(),
            boost::asio::buffer(*sendBuffer),
            [this, connection, sendBuffer](boost::system::error_code ec, std::size_t)
            {
                if (ec)
                {
                    connection->Disconnect();
                    RemoveConnection(connection->GetId()); // 🌟 统一收拢
                    connection->EndWrite();
                    return;
                }

                connection->EndWrite();
                StartWriteLoop(connection);
            });
    }
}