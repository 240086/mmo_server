// /home/xj/projects/mmo_server/server_core/infrastructure/Net/include/mmo/infrastructure/Net/NetServer.hpp
#pragma once

#include <atomic>
#include <memory>
#include <cstddef>
#include <optional>

#include <mmo/infrastructure/Net/INetServer.hpp>

namespace mmo::infrastructure::net
{
    class NetServer final : public INetServer
    {
    public:
        NetServer();
        ~NetServer() override; // 🌟 Pimpl 要求析构函数必须在 .cpp 中实现

        NetServer(const NetServer &) = delete;
        NetServer &operator=(const NetServer &) = delete;

        NetServer(NetServer &&) = delete;
        NetServer &operator=(NetServer &&) = delete;

    public:
        bool Start(std::uint16_t port) override;
        void Stop() override;
        bool IsRunning() const noexcept override;
        bool Send(const OutboundPacket &packet) override;
        bool PollPacket(ReceivedPacket &packet) override;
        std::size_t ConnectionCount() const noexcept override;

    private:
        std::atomic<bool> running_{false};

        // 🌟 核心扩展：面向对象解耦的终极武器 —— Pimpl
        // 将所有的 boost::asio::io_context, ip::tcp::acceptor 隔离在 .cpp 的 Impl 结构体中
        struct Impl;
        std::unique_ptr<Impl> impl_;
    };
}