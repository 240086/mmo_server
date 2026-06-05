// /home/xj/projects/mmo_server/server_core/infrastructure/packet/include/PacketTypes.hpp
#pragma once

#include <cstdint>

namespace mmo::infrastructure::packet
{
    // 强类型别名，便于后续全局无缝升级位宽
    using PacketId      = std::uint16_t; // 最大支持 65535 个协议号
    using PacketSize    = std::uint32_t; // 单包最大支持 4GB payload（实际受限于 MaxPacketSize）
    using PacketVersion = std::uint16_t; // 版本控制号
}