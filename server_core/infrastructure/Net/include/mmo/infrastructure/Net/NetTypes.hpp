// /home/xj/projects/mmo_server/server_core/infrastructure/Net/include/mmo/infrastructure/Net/NetTypes.hpp
#pragma once

#include <cstdint>
#include <vector>

#include <mmo/infrastructure/Net/ConnectionId.hpp>

namespace mmo::infrastructure::net
{
    using Byte = std::uint8_t;

    using ByteBuffer = std::vector<Byte>;

    struct ReceivedPacket
    {
        ConnectionId connectionId{InvalidConnectionId};
        std::uint16_t packetId{0};
        ByteBuffer payload;
    };

    struct OutboundPacket
    {
        ConnectionId connectionId{InvalidConnectionId};
        std::uint16_t packetId{0};
        ByteBuffer payload;
    };
}