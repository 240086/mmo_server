#include <gtest/gtest.h>

#include <cstdint>
#include <string>

#include <mmo/infrastructure/packet/PacketBuffer.hpp>
#include <mmo/infrastructure/packet/PacketReader.hpp>
#include <mmo/infrastructure/packet/PacketView.hpp>
#include <mmo/infrastructure/packet/PacketWriter.hpp>

#include <mmo/infrastructure/serialization/BinaryReader.hpp>
#include <mmo/infrastructure/serialization/BinaryWriter.hpp>

namespace
{
    using namespace mmo::infrastructure::packet;
    using namespace mmo::infrastructure::serialization;
}

TEST(SerializationBoundaryTests, ReadBeyondEndFails)
{
    PacketBuffer buffer(64);

    PacketWriter packetWriter(buffer);
    BinaryWriter writer(packetWriter);

    const std::uint32_t value = 123456u;

    ASSERT_TRUE(writer.Write(value));

    PacketView view(
        buffer.Data(),
        packetWriter.GetOffset());

    PacketReader packetReader(view);
    BinaryReader reader(packetReader);

    std::uint32_t first{};
    std::uint32_t second{};

    EXPECT_TRUE(reader.Read(first));
    EXPECT_EQ(first, value);

    EXPECT_FALSE(reader.Read(second));
}

TEST(SerializationBoundaryTests, EmptyStreamReadFails)
{
    PacketView view(nullptr, 0);

    PacketReader packetReader(view);
    BinaryReader reader(packetReader);

    std::uint32_t value{};

    EXPECT_FALSE(reader.Read(value));
}

TEST(SerializationBoundaryTests, EmptyStreamStringReadFails)
{
    PacketView view(nullptr, 0);

    PacketReader packetReader(view);
    BinaryReader reader(packetReader);

    std::string value;

    EXPECT_FALSE(reader.ReadString(value));
}

TEST(SerializationBoundaryTests, TruncatedPrimitiveReadFails)
{
    PacketBuffer buffer(sizeof(std::uint16_t));

    PacketWriter packetWriter(buffer);
    BinaryWriter writer(packetWriter);

    const std::uint16_t value = 777;

    ASSERT_TRUE(writer.Write(value));

    PacketView truncatedView(
        buffer.Data(),
        1);

    PacketReader packetReader(truncatedView);
    BinaryReader reader(packetReader);

    std::uint16_t output{};

    EXPECT_FALSE(reader.Read(output));
}

TEST(SerializationBoundaryTests, TruncatedStringPayloadFails)
{
    PacketBuffer buffer(64);

    PacketWriter packetWriter(buffer);
    BinaryWriter writer(packetWriter);

    ASSERT_TRUE(
        writer.WriteString("abcdef"));

    constexpr std::size_t truncatedSize =
        sizeof(std::uint32_t) + 2;

    PacketView truncatedView(
        buffer.Data(),
        truncatedSize);

    PacketReader packetReader(truncatedView);
    BinaryReader reader(packetReader);

    std::string output;

    EXPECT_FALSE(
        reader.ReadString(output));
}

TEST(SerializationBoundaryTests, OversizedLengthStringFails)
{
    PacketBuffer buffer(16);

    PacketWriter packetWriter(buffer);

    const std::uint32_t invalidLength =
        0xFFFFFFFFu;

    ASSERT_TRUE(
        packetWriter.Write(invalidLength));

    PacketView view(
        buffer.Data(),
        packetWriter.GetOffset());

    PacketReader packetReader(view);
    BinaryReader reader(packetReader);

    std::string value;

    EXPECT_FALSE(
        reader.ReadString(value));
}

TEST(SerializationBoundaryTests, NullDestinationReadFails)
{
    PacketBuffer buffer(16);

    PacketWriter packetWriter(buffer);
    BinaryWriter writer(packetWriter);

    const std::uint32_t value = 42;

    ASSERT_TRUE(writer.Write(value));

    PacketView view(
        buffer.Data(),
        packetWriter.GetOffset());

    PacketReader packetReader(view);
    BinaryReader reader(packetReader);

    EXPECT_FALSE(
        reader.ReadBytes(nullptr, sizeof(value)));
}

TEST(SerializationBoundaryTests, ZeroLengthReadSucceeds)
{
    PacketBuffer buffer(8);

    PacketWriter packetWriter(buffer);

    PacketView view(
        buffer.Data(),
        0);

    PacketReader packetReader(view);
    BinaryReader reader(packetReader);

    EXPECT_TRUE(
        reader.ReadBytes(nullptr, 0));
}