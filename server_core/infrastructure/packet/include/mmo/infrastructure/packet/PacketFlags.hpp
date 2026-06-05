// /home/xj/projects/mmo_server/server_core/infrastructure/packet/include/PacketFlags.hpp
#pragma once

#include <cstdint>
#include <type_traits>

namespace mmo::infrastructure::packet
{
    enum class PacketFlags : std::uint16_t
    {
        None = 0,
        Compressed = 1 << 0, // 数据载荷是否经过 Zstd/Lz4 压缩
        Encrypted = 1 << 1,  // 数据载荷是否经过流式加密（如 AES-GCM / ChaCha20）
        Reliable = 1 << 2,   // 针对 UDP 内部底层（如 KCP/ENet）的可靠性传输标记
    };

    // 🧠 架构师补充：优雅实现 enum class 的有安全型位运算重载，免去到处进行 static_cast 的痛苦
    inline constexpr PacketFlags operator|(PacketFlags lhs, PacketFlags rhs) noexcept
    {
        return static_cast<PacketFlags>(
            static_cast<std::underlying_type_t<PacketFlags>>(lhs) |
            static_cast<std::underlying_type_t<PacketFlags>>(rhs));
    }

    inline constexpr PacketFlags operator&(PacketFlags lhs, PacketFlags rhs) noexcept
    {
        return static_cast<PacketFlags>(
            static_cast<std::underlying_type_t<PacketFlags>>(lhs) &
            static_cast<std::underlying_type_t<PacketFlags>>(rhs));
    }

    inline constexpr PacketFlags &operator|=(PacketFlags &lhs, PacketFlags rhs) noexcept
    {
        lhs = lhs | rhs;
        return lhs;
    }

    inline constexpr PacketFlags &operator&=(PacketFlags &lhs, PacketFlags rhs) noexcept
    {
        lhs = lhs & rhs;
        return lhs;
    }

    inline constexpr bool HasFlag(PacketFlags flags, PacketFlags target) noexcept
    {
        return (flags & target) != PacketFlags::None;
    }
}