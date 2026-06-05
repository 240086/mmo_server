// /home/xj/projects/mmo_server/server_core/tests/infrastructure/packet/PacketLargePayloadTests.cpp
#include <gtest/gtest.h>

#include <vector>

#include <mmo/infrastructure/packet/PacketBuffer.hpp>
#include <mmo/infrastructure/packet/PacketReader.hpp>
#include <mmo/infrastructure/packet/PacketWriter.hpp>
#include <mmo/infrastructure/packet/PacketView.hpp>

using namespace mmo::infrastructure::packet;

TEST(PacketLargePayloadTests, LargePayloadRoundTrip)
{
    constexpr std::size_t PayloadSize =
        64 * 1024;

    std::vector<std::byte> source(
        PayloadSize);

    for (std::size_t i = 0; i < PayloadSize; ++i)
    {
        source[i] =
            static_cast<std::byte>(
                i % 255);
    }

    PacketBuffer buffer;

    PacketWriter writer(buffer);

    ASSERT_TRUE(
        writer.WriteBytes(
            source.data(),
            source.size()));

    PacketReader reader(
        PacketView(
            buffer.Data(),
            buffer.Size()));

    std::vector<std::byte> destination(
        PayloadSize);

    ASSERT_TRUE(
        reader.ReadBytes(
            destination.data(),
            destination.size()));

    EXPECT_EQ(
        source,
        destination);
}