// /home/xj/projects/mmo_server/server_core/infrastructure/Net/include/mmo/infrastructure/Net/TcpConnection.hpp
#pragma once

#include <atomic>
#include <deque>
#include <memory>
#include <mutex>
#include <cstddef>

#include <boost/asio/ip/tcp.hpp>
#include <mmo/infrastructure/Net/IConnection.hpp>
#include <mmo/infrastructure/Net/NetTypes.hpp>

namespace mmo::infrastructure::net
{
    class TcpConnection final
        : public IConnection,
          public std::enable_shared_from_this<TcpConnection>
    {
    public:
        using Socket = boost::asio::ip::tcp::socket;

        // 🌟 核心防御：每个连接最大的待发送包积压数（背压线）
        static constexpr std::size_t kMaxPendingSendPackets = 1024;

    public:
        TcpConnection(ConnectionId connectionId, Socket socket);
        ~TcpConnection() override = default;

        TcpConnection(const TcpConnection &) = delete;
        TcpConnection &operator=(const TcpConnection &) = delete;

    public:
        ConnectionId GetId() const noexcept override;
        bool IsConnected() const noexcept override;
        void Disconnect() override;

    public:
        Socket &GetSocket() noexcept;
        const Socket &GetSocket() const noexcept;
        ByteBuffer &ReceiveBuffer() noexcept;

        bool EnqueueSend(ByteBuffer packet);
        bool TryPopSend(ByteBuffer &packet);

        bool TryBeginWrite();
        void EndWrite();
        bool HasPendingSend() const;

    private:
        const ConnectionId connectionId_;
        Socket socket_;
        std::atomic<bool> connected_{true};

        ByteBuffer receiveBuffer_;

        std::deque<ByteBuffer> sendQueue_;
        mutable std::mutex sendMutex;

        std::atomic<bool> writeInProgress_{false};
    };
}