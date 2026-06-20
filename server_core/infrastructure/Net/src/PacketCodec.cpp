// /home/xj/projects/mmo_server/server_core/infrastructure/Net/src/PacketCodec.cpp
// server_core/infrastructure/Net/src/PacketCodec.cpp
#include <mmo/infrastructure/Net/PacketCodec.hpp>
#include <cstring>

namespace mmo::infrastructure::net
{
    namespace
    {
        constexpr std::uint32_t kHeaderSize = sizeof(PacketFrame); // 严格等于 6
        constexpr std::uint32_t kMaxPacketSize = 64 * 1024;        // 64KB 边界防御
    }

    ByteBuffer PacketCodec::Encode(
        std::uint16_t packetId,
        const ByteBuffer &payload)
    {
        PacketFrame frame;
        // 🌟 严格对齐冻结语义：只代表 payload 自身的长度
        frame.payloadLength = static_cast<std::uint32_t>(payload.size());
        frame.packetId = packetId;

        ByteBuffer output;
        output.resize(kHeaderSize + payload.size());

        // 写入 6 字节固定包头
        std::memcpy(output.data(), &frame, kHeaderSize);

        // 写入裸 Payload
        if (!payload.empty())
        {
            std::memcpy(output.data() + kHeaderSize, payload.data(), payload.size());
        }

        return output;
    }

    bool PacketCodec::Decode(
        ByteBuffer &receiveBuffer,
        ReceivedPacket &outPacket)
    {
        // 连一个完整的包头都不够，直接 fail-fast
        if (receiveBuffer.size() < kHeaderSize)
        {
            return false;
        }

        PacketFrame frame;
        std::memcpy(&frame, receiveBuffer.data(), kHeaderSize);

        // 缓冲区溢出与畸形包攻击防御
        if (frame.payloadLength > kMaxPacketSize)
        {
            receiveBuffer.clear(); // 严重协议违规，直接清空整个缓冲区并等待断开
            return false;
        }

        // 🌟 公式极度简化：总长度 = 6 字节头部 + 纯 payload 长度
        const std::size_t totalPacketSize = kHeaderSize + frame.payloadLength;

        // 流式粘包处理：当前缓冲区里的数据还不够一个完整的业务包
        if (receiveBuffer.size() < totalPacketSize)
        {
            return false;
        }

        // 🌟 核心优化：复用内存，避免产生任何临时 ReceivedPacket 对象
        outPacket.packetId = frame.packetId;

        // vector.resize() 在 capacity 足够时，仅仅是移动 size 指针，零堆内存分配！
        outPacket.payload.resize(frame.payloadLength);

        if (frame.payloadLength > 0)
        {
            std::memcpy(
                outPacket.payload.data(),
                receiveBuffer.data() + kHeaderSize,
                frame.payloadLength);
        }

        // 从 TCP 接收流缓冲区中弹出已消费的数据段
        receiveBuffer.erase(
            receiveBuffer.begin(),
            receiveBuffer.begin() + totalPacketSize);

        return true;
    }
}