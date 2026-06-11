// /home/xj/projects/mmo_server/server_core/tests/infrastructure/serialization/TestHelpers.hpp
#pragma once

#include <mmo/infrastructure/packet/PacketBuffer.hpp>
#include <mmo/infrastructure/packet/PacketReader.hpp>
#include <mmo/infrastructure/packet/PacketView.hpp>

namespace test
{
    inline mmo::infrastructure::packet::PacketReader
    CreateReader(
        const mmo::infrastructure::packet::PacketBuffer &buffer)
    {
        using namespace mmo::infrastructure::packet;

        return PacketReader{
            PacketView{
                buffer.Data(),
                buffer.Size()}};
    }
}