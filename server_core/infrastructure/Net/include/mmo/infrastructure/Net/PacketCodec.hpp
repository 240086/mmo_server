// /home/xj/projects/mmo_server/server_core/infrastructure/Net/include/mmo/infrastructure/Net/PacketCodec.hpp
#pragma once

#include <mmo/infrastructure/Net/NetTypes.hpp>
#include <mmo/infrastructure/Net/PacketFrame.hpp>

namespace mmo::infrastructure::net
{
    class PacketCodec final
    {
    public:
        PacketCodec() = delete;

        // 编码：将业务层包塞入连续的 TCP 字节流
        static ByteBuffer Encode(
            std::uint16_t packetId,
            const ByteBuffer &payload);

        // 🌟 架构进化：改用 bool 传参，完全复用外部传入的 packet 内部 vector 内存桶
        static bool Decode(
            ByteBuffer &receiveBuffer,
            ReceivedPacket &outPacket);
    };
}