// home/xj/projects/mmo_server/server_core/infrastructure/packet/include/PacketConstants.hpp
#pragma once

#include <cstddef>
#include <cstdint>

namespace mmo::infrastructure::packet
{
    // MMO 协议幻数 (ASCII: "MMO1")
    constexpr std::uint32_t kPacketMagic = 0x4D4D4F31;

    // 默认初始缓冲区容量（4KB，贴合绝大多数操作系统的物理内存页大小，规避初次分配抖动）
    constexpr std::size_t kDefaultPacketCapacity = 4096;

    // 工业级网络防大包攻击红线（64KB），超过此限制的单包直接在网关层判定为非法攻击并断开
    constexpr std::size_t kMaxPacketSize = 64 * 1024;

    // 当前演进轴上的主版本号
    constexpr std::uint16_t kProtocolVersion = 1;
}