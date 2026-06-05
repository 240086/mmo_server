// /home/xj/projects/mmo_server/server_core/infrastructure/packet/include/PacketHeader.hpp
#pragma once

#include <cstdint>
#include <type_traits>
#include <mmo/infrastructure/packet/PacketTypes.hpp>
#include <mmo/infrastructure/packet/PacketFlags.hpp>

namespace mmo::infrastructure::packet
{
// 💡 架构师级设计：强行启用 1 字节物理对齐，杜绝不同平台/编译器产生隐式隐式内存空洞（Padding）
#pragma pack(push, 1)
    struct PacketHeader
    {
        std::uint32_t magic;    // 4 Bytes
        PacketVersion version;  // 2 Bytes
        PacketId packetId;      // 2 Bytes
        PacketFlags flags;      // 2 Bytes
        PacketSize payloadSize; // 4 Bytes
        std::uint16_t reserved; // 2 Bytes

        // 未来：
        // compression type
        // trace id
        // rpc extension
        // protocol evolution
    };
#pragma pack(pop)

    // 静态内存断言护航：确保帧头数据在网络传输中具有确定性的物理尺寸
    static_assert(sizeof(PacketFlags) == 2);
    static_assert(sizeof(PacketHeader) == 16, "PacketHeader size alignment error! Must be exactly 16 bytes.");
    static_assert(std::is_standard_layout_v<PacketHeader>, "PacketHeader must be a standard layout POD!");
    static_assert(std::is_trivially_copyable_v<PacketHeader>, "PacketHeader must be trivially copyable for direct memcpy!");
}