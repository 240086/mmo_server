// /home/xj/projects/mmo_server/server_core/tests/infrastructure/Net/PacketCodecTests.cpp
#include <gtest/gtest.h>

#include <mmo/infrastructure/Net/PacketCodec.hpp>

using namespace mmo::infrastructure::net;

TEST(PacketCodecTests, EncodeDecodeSinglePacket)
{
    ByteBuffer payload{1, 2, 3, 4, 5};

    auto encoded =
        PacketCodec::Encode(
            1001,
            payload);

    ByteBuffer stream = encoded;

    ReceivedPacket packet;

    ASSERT_TRUE(
        PacketCodec::Decode(
            stream,
            packet));

    EXPECT_EQ(packet.packetId, 1001);
    EXPECT_EQ(packet.payload, payload);
}

TEST(PacketCodecTests, DecodePartialPacketReturnsFalse)
{
    ByteBuffer payload{1, 2, 3};

    auto encoded =
        PacketCodec::Encode(
            10,
            payload);

    encoded.resize(4);

    ReceivedPacket packet;

    EXPECT_FALSE(
        PacketCodec::Decode(
            encoded,
            packet));
}

TEST(PacketCodecTests, DecodeStickyPackets)
{
    ByteBuffer stream;

    auto p1 =
        PacketCodec::Encode(
            1,
            {10});

    auto p2 =
        PacketCodec::Encode(
            2,
            {20});

    stream.insert(
        stream.end(),
        p1.begin(),
        p1.end());

    stream.insert(
        stream.end(),
        p2.begin(),
        p2.end());

    ReceivedPacket packet;

    ASSERT_TRUE(
        PacketCodec::Decode(
            stream,
            packet));

    EXPECT_EQ(packet.packetId, 1);

    ASSERT_TRUE(
        PacketCodec::Decode(
            stream,
            packet));

    EXPECT_EQ(packet.packetId, 2);

    EXPECT_TRUE(stream.empty());
}

TEST(PacketCodecTests, EmptyPayload)
{
    ByteBuffer payload;

    auto encoded =
        PacketCodec::Encode(
            999,
            payload);

    ByteBuffer stream = encoded;

    ReceivedPacket packet;

    ASSERT_TRUE(
        PacketCodec::Decode(
            stream,
            packet));

    EXPECT_EQ(packet.packetId, 999);
    EXPECT_TRUE(packet.payload.empty());
}