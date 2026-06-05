// /home/xj/projects/mmo_server/server_core/tests/infrastructure/packet/PacketRoundTripTests.cpp
#include <gtest/gtest.h>

#include <string>
#include <cstdint>

#include <mmo/infrastructure/packet/PacketBuffer.hpp>
#include <mmo/infrastructure/packet/PacketView.hpp>
#include <mmo/infrastructure/packet/PacketReader.hpp>
#include <mmo/infrastructure/packet/PacketWriter.hpp>

using namespace mmo::infrastructure::packet;

TEST(PacketRoundTripTests, PrimitiveRoundTrip)
{
    PacketBuffer buffer;

    PacketWriter writer(buffer);

    std::uint32_t a = 12345;
    std::uint64_t b = 987654321ULL;
    float c = 3.14159f;

    ASSERT_TRUE(writer.Write(a));
    ASSERT_TRUE(writer.Write(b));
    ASSERT_TRUE(writer.Write(c));

    PacketView view(buffer.Data(), buffer.Size());
    PacketReader reader(view);

    std::uint32_t ra;
    std::uint64_t rb;
    float rc;

    ASSERT_TRUE(reader.Read(ra));
    ASSERT_TRUE(reader.Read(rb));
    ASSERT_TRUE(reader.Read(rc));

    EXPECT_EQ(ra, a);
    EXPECT_EQ(rb, b);
    EXPECT_FLOAT_EQ(rc, c);
}

TEST(PacketRoundTripTests, StringRoundTrip)
{
    PacketBuffer buffer;

    PacketWriter writer(buffer);

    const std::string expected =
        "Industrial MMO Server";

    ASSERT_TRUE(
        writer.WriteString(expected));

    PacketReader reader(
        PacketView(
            buffer.Data(),
            buffer.Size()));

    std::string actual;

    ASSERT_TRUE(
        reader.ReadString(actual));

    EXPECT_EQ(actual, expected);
}

TEST(PacketRoundTripTests, MixedPayloadRoundTrip)
{
    PacketBuffer buffer;

    PacketWriter writer(buffer);

    std::uint32_t id = 1001;
    std::string name = "Knight";
    std::uint64_t hp = 50000;

    ASSERT_TRUE(writer.Write(id));
    ASSERT_TRUE(writer.WriteString(name));
    ASSERT_TRUE(writer.Write(hp));

    PacketReader reader(
        PacketView(
            buffer.Data(),
            buffer.Size()));

    std::uint32_t rid;
    std::string rname;
    std::uint64_t rhp;

    ASSERT_TRUE(reader.Read(rid));
    ASSERT_TRUE(reader.ReadString(rname));
    ASSERT_TRUE(reader.Read(rhp));

    EXPECT_EQ(rid, id);
    EXPECT_EQ(rname, name);
    EXPECT_EQ(rhp, hp);
}