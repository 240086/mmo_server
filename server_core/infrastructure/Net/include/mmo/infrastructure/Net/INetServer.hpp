// /home/xj/projects/mmo_server/server_core/infrastructure/Net/include/mmo/infrastructure/Net/INetServer.hpp
#pragma once

#include <cstddef>
#include <cstdint>
#include <optional>

#include <mmo/infrastructure/Net/NetTypes.hpp>

namespace mmo::infrastructure::net
{
    class INetServer
    {
    public:
        virtual ~INetServer() = default;

        virtual bool Start(std::uint16_t port) = 0;
        virtual void Stop() = 0;
        virtual bool IsRunning() const noexcept = 0;
        virtual bool Send(const OutboundPacket &packet) = 0;

        // 🌟 MMO 热路径极致优化：抹平一切临时对象开销
        virtual bool PollPacket(ReceivedPacket &packet) = 0;

        virtual std::size_t ConnectionCount() const noexcept = 0;
    };
}