#include <gtest/gtest.h>

#include <cstdint>

#include <mmo/infrastructure/packet/PacketBuffer.hpp>
#include <mmo/infrastructure/packet/PacketWriter.hpp>

#include <mmo/infrastructure/serialization/BinaryWriter.hpp>

using namespace mmo::infrastructure;

namespace
{
    packet::PacketBuffer CreateBuffer()
    {
        return packet::PacketBuffer(1024);
    }
}

TEST(BinaryWriterTests, InitialOffsetIsZero)
{
    auto buffer = CreateBuffer();

    packet::PacketWriter packetWriter(buffer);

    serialization::BinaryWriter writer(packetWriter);

    EXPECT_EQ(
        packetWriter.GetOffset(),
        0u);
}

TEST(BinaryWriterTests, WritePrimitiveAdvancesOffset)
{
    auto buffer = CreateBuffer();

    packet::PacketWriter packetWriter(buffer);

    serialization::BinaryWriter writer(packetWriter);

    uint32_t value = 123456u;

    ASSERT_TRUE(
        writer.Write(value));

    EXPECT_EQ(
        packetWriter.GetOffset(),
        sizeof(uint32_t));
}

TEST(BinaryWriterTests, WriteStringAdvancesOffset)
{
    auto buffer = CreateBuffer();

    packet::PacketWriter packetWriter(buffer);

    serialization::BinaryWriter writer(packetWriter);

    ASSERT_TRUE(
        writer.WriteString("abc"));

    EXPECT_EQ(
        packetWriter.GetOffset(),
        sizeof(uint32_t) + 3);
}

TEST(BinaryWriterTests, WriteBytesAcceptsZeroLength)
{
    auto buffer = CreateBuffer();

    packet::PacketWriter packetWriter(buffer);

    serialization::BinaryWriter writer(packetWriter);

    EXPECT_TRUE(
        writer.WriteBytes(nullptr, 0));
}

TEST(BinaryWriterTests, WriteBytesRejectsNullPointer)
{
    auto buffer = CreateBuffer();

    packet::PacketWriter packetWriter(buffer);

    serialization::BinaryWriter writer(packetWriter);

    EXPECT_FALSE(
        writer.WriteBytes(nullptr, 16));
}

TEST(BinaryWriterTests, MultipleWritesRemainSequential)
{
    auto buffer = CreateBuffer();

    packet::PacketWriter packetWriter(buffer);

    serialization::BinaryWriter writer(packetWriter);

    ASSERT_TRUE(writer.Write(uint32_t{1}));
    ASSERT_TRUE(writer.Write(uint64_t{2}));
    ASSERT_TRUE(writer.Write(uint16_t{3}));

    EXPECT_EQ(
        packetWriter.GetOffset(),
        sizeof(uint32_t) + sizeof(uint64_t) + sizeof(uint16_t));
}