// /home/xj/projects/mmo_server/server_core/tests/infrastructure/packet/PacketBoundaryTests.cpp
#include <gtest/gtest.h>

#include <vector>

#include <mmo/infrastructure/packet/PacketWriter.hpp>
#include <mmo/infrastructure/packet/PacketConstants.hpp>

using namespace mmo::infrastructure::packet;

TEST(PacketBoundaryTests, AcceptsMaxPacketSize)
{
    PacketBuffer buffer;
    PacketWriter writer(buffer);

    std::vector<std::byte> payload(
        kMaxPacketSize);

    EXPECT_TRUE(
        writer.WriteBytes(
            payload.data(),
            payload.size()));
}

TEST(PacketBoundaryTests, RejectsBeyondMaxPacketSize)
{
    PacketBuffer buffer;
    PacketWriter writer(buffer);

    std::vector<std::byte> payload(
        kMaxPacketSize + 1);

    EXPECT_FALSE(
        writer.WriteBytes(
            payload.data(),
            payload.size()));
}

TEST(PacketBoundaryTests, CursorUnchangedAfterFailure)
{
    PacketBuffer buffer;
    PacketWriter writer(buffer);

    std::vector<std::byte> payload(
        kMaxPacketSize + 1);

    EXPECT_FALSE(
        writer.WriteBytes(
            payload.data(),
            payload.size()));

    EXPECT_EQ(
        writer.GetOffset(),
        0u);
}