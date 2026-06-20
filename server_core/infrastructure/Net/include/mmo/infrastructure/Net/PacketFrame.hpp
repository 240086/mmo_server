// /home/xj/projects/mmo_server/server_core/infrastructure/Net/include/mmo/infrastructure/Net/PacketFrame.hpp
#pragma once

#include <cstdint>

namespace mmo::infrastructure::net
{
#pragma pack(push, 1)

    struct PacketFrame
    {
        /**
         * payload bytes only
         */
        std::uint32_t payloadLength;

        std::uint16_t packetId;
    };

#pragma pack(pop)

    static_assert(
        sizeof(PacketFrame) == 6,
        "PacketFrame layout mismatch");
}