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

TEST(SerializationRoundTripTests, MixedPrimitiveAndStringRoundTrip)
{
    PacketBuffer buffer(4096);

    PacketWriter packetWriter(buffer);
    BinaryWriter writer(packetWriter);

    const std::uint32_t id = 1001;
    const bool online = true;
    const float hp = 95.5f;
    const double mana = 1234.567;
    const std::string name = "Knight";

    ASSERT_TRUE(writer.Write(id));
    ASSERT_TRUE(writer.Write(online));
    ASSERT_TRUE(writer.Write(hp));
    ASSERT_TRUE(writer.Write(mana));
    ASSERT_TRUE(writer.WriteString(name));

    PacketView view(
        buffer.Data(),
        packetWriter.GetOffset());

    PacketReader packetReader(view);
    BinaryReader reader(packetReader);

    std::uint32_t readId{};
    bool readOnline{};
    float readHp{};
    double readMana{};
    std::string readName;

    ASSERT_TRUE(reader.Read(readId));
    ASSERT_TRUE(reader.Read(readOnline));
    ASSERT_TRUE(reader.Read(readHp));
    ASSERT_TRUE(reader.Read(readMana));
    ASSERT_TRUE(reader.ReadString(readName));

    EXPECT_EQ(readId, id);
    EXPECT_EQ(readOnline, online);
    EXPECT_FLOAT_EQ(readHp, hp);
    EXPECT_DOUBLE_EQ(readMana, mana);
    EXPECT_EQ(readName, name);
}

TEST(SerializationRoundTripTests, MultipleSequentialObjectsRoundTrip)
{
    PacketBuffer buffer(4096);

    PacketWriter packetWriter(buffer);
    BinaryWriter writer(packetWriter);

    for (std::uint32_t i = 0; i < 1000; ++i)
    {
        ASSERT_TRUE(
            writer.Write(i));
    }

    PacketView view(
        buffer.Data(),
        packetWriter.GetOffset());

    PacketReader packetReader(view);
    BinaryReader reader(packetReader);

    for (std::uint32_t i = 0; i < 1000; ++i)
    {
        std::uint32_t value{};

        ASSERT_TRUE(
            reader.Read(value));

        EXPECT_EQ(
            value,
            i);
    }
}

TEST(SerializationRoundTripTests, MultipleStringsRoundTrip)
{
    PacketBuffer buffer(4096);

    PacketWriter packetWriter(buffer);
    BinaryWriter writer(packetWriter);

    ASSERT_TRUE(writer.WriteString("Alpha"));
    ASSERT_TRUE(writer.WriteString("Beta"));
    ASSERT_TRUE(writer.WriteString("Gamma"));
    ASSERT_TRUE(writer.WriteString("Delta"));

    PacketView view(
        buffer.Data(),
        packetWriter.GetOffset());

    PacketReader packetReader(view);
    BinaryReader reader(packetReader);

    std::string a;
    std::string b;
    std::string c;
    std::string d;

    ASSERT_TRUE(reader.ReadString(a));
    ASSERT_TRUE(reader.ReadString(b));
    ASSERT_TRUE(reader.ReadString(c));
    ASSERT_TRUE(reader.ReadString(d));

    EXPECT_EQ(a, "Alpha");
    EXPECT_EQ(b, "Beta");
    EXPECT_EQ(c, "Gamma");
    EXPECT_EQ(d, "Delta");
}

TEST(SerializationRoundTripTests, ReaderConsumesEntireStream)
{
    PacketBuffer buffer(256);

    PacketWriter packetWriter(buffer);
    BinaryWriter writer(packetWriter);

    ASSERT_TRUE(writer.Write(std::uint32_t{123}));
    ASSERT_TRUE(writer.Write(std::uint64_t{456}));

    PacketView view(
        buffer.Data(),
        packetWriter.GetOffset());

    PacketReader packetReader(view);
    BinaryReader reader(packetReader);

    std::uint32_t a{};
    std::uint64_t b{};

    ASSERT_TRUE(reader.Read(a));
    ASSERT_TRUE(reader.Read(b));

    EXPECT_EQ(
        packetReader.GetRemainingBytes(),
        0u);
}

TEST(SerializationRoundTripTests, SerializationIsDeterministicAcrossRuns)
{
    PacketBuffer lhs(1024);
    PacketBuffer rhs(1024);

    {
        PacketWriter packetWriter(lhs);
        BinaryWriter writer(packetWriter);

        ASSERT_TRUE(writer.Write(std::uint32_t{777}));
        ASSERT_TRUE(writer.WriteString("MMO"));
    }

    {
        PacketWriter packetWriter(rhs);
        BinaryWriter writer(packetWriter);

        ASSERT_TRUE(writer.Write(std::uint32_t{777}));
        ASSERT_TRUE(writer.WriteString("MMO"));
    }

    EXPECT_EQ(
        lhs.Size(),
        rhs.Size());

    EXPECT_EQ(
        std::memcmp(
            lhs.Data(),
            rhs.Data(),
            lhs.Size()),
        0);
}