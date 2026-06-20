#include <mmo/infrastructure/Net/TcpConnection.hpp>

namespace mmo::infrastructure::net
{
    TcpConnection::TcpConnection(ConnectionId connectionId, Socket socket)
        : connectionId_(connectionId),
          socket_(std::move(socket))
    {
    }

    ConnectionId TcpConnection::GetId() const noexcept { return connectionId_; }

    bool TcpConnection::IsConnected() const noexcept 
    { 
        return connected_.load(std::memory_order_acquire); 
    }

    void TcpConnection::Disconnect()
    {
        bool expected = true;
        if (!connected_.compare_exchange_strong(expected, false, std::memory_order_acq_rel))
        {
            return;
        }

        boost::system::error_code ec;
        socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ec);
        socket_.close(ec);
    }

    TcpConnection::Socket &TcpConnection::GetSocket() noexcept { return socket_; }
    const TcpConnection::Socket &TcpConnection::GetSocket() const noexcept { return socket_; }
    ByteBuffer &TcpConnection::ReceiveBuffer() noexcept { return receiveBuffer_; }

    bool TcpConnection::EnqueueSend(ByteBuffer packet)
    {
        if (!connected_.load(std::memory_order_acquire))
        {
            return false;
        }

        std::scoped_lock lock(sendMutex);
        
        // 🌟 背压检查：如果队列积压过大，说明客户端拒绝收包或链路已死，拒绝写入并触发上层惩罚
        if (sendQueue_.size() >= kMaxPendingSendPackets)
        {
            return false; 
        }

        sendQueue_.push_back(std::move(packet));
        return true;
    }

    bool TcpConnection::TryPopSend(ByteBuffer &packet)
    {
        std::scoped_lock lock(sendMutex);
        if (sendQueue_.empty())
        {
            return false;
        }

        packet = std::move(sendQueue_.front());
        sendQueue_.pop_front();
        return true;
    }

    bool TcpConnection::TryBeginWrite()
    {
        bool expected = false;
        return writeInProgress_.compare_exchange_strong(expected, true, std::memory_order_acq_rel);
    }

    void TcpConnection::EndWrite()
    {
        writeInProgress_.store(false, std::memory_order_release);
    }

    bool TcpConnection::HasPendingSend() const
    {
        std::scoped_lock lock(sendMutex);
        return !sendQueue_.empty();
    }
}