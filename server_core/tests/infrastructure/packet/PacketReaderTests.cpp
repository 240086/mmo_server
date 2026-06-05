// /home/xj/projects/mmo_server/server_core/tests/infrastructure/packet/PacketReaderTests.cpp
#include <gtest/gtest.h>

#include <array>
#include <cstdint>
#include <string>

#include <mmo/infrastructure/packet/PacketReader.hpp>

using namespace mmo::infrastructure::packet;

TEST(PacketReaderTests, InitialOffsetIsZero)
{
    std::array<std::byte, 16> buffer{};

    PacketReader reader(
        PacketView(
            buffer.data(),
            buffer.size()));

    EXPECT_EQ(
        reader.GetOffset(),
        0u);
}

TEST(PacketReaderTests, ReadUInt32)
{
    const std::uint32_t value =
        123456u;

    std::array<std::byte, sizeof(value)> buffer{};

    std::memcpy(
        buffer.data(),
        &value,
        sizeof(value));

    PacketReader reader(
        PacketView(
            buffer.data(),
            buffer.size()));

    std::uint32_t result = 0;

    EXPECT_TRUE(
        reader.Read(result));

    EXPECT_EQ(
        result,
        value);

    EXPECT_EQ(
        reader.GetOffset(),
        sizeof(value));
}

TEST(PacketReaderTests, ReadBytes)
{
    std::array<std::byte, 8> source{};

    for (std::size_t i = 0; i < source.size(); ++i)
    {
        source[i] =
            static_cast<std::byte>(i);
    }

    PacketReader reader(
        PacketView(
            source.data(),
            source.size()));

    std::array<std::byte, 8> destination{};

    EXPECT_TRUE(
        reader.ReadBytes(
            destination.data(),
            destination.size()));

    EXPECT_EQ(
        source,
        destination);
}

TEST(PacketReaderTests, ReadFailsWhenBufferTooSmall)
{
    std::array<std::byte, 2> buffer{};

    PacketReader reader(
        PacketView(
            buffer.data(),
            buffer.size()));

    std::uint32_t value = 0;

    EXPECT_FALSE(
        reader.Read(value));

    EXPECT_EQ(
        reader.GetOffset(),
        0u);
}

TEST(PacketReaderTests, RemainingBytesUpdatedAfterRead)
{
    std::uint32_t value = 42;

    std::array<std::byte, sizeof(value)> buffer{};

    std::memcpy(
        buffer.data(),
        &value,
        sizeof(value));

    PacketReader reader(
        PacketView(
            buffer.data(),
            buffer.size()));

    EXPECT_EQ(
        reader.GetRemainingBytes(),
        sizeof(value));

    std::uint32_t result;

    EXPECT_TRUE(
        reader.Read(result));

    EXPECT_EQ(
        reader.GetRemainingBytes(),
        0u);
}

TEST(PacketReaderTests, ReadString)
{
    const std::string text =
        "MMO";

    std::array<std::byte, 64> buffer{};

    std::uint32_t length =
        static_cast<std::uint32_t>(
            text.size());

    std::memcpy(
        buffer.data(),
        &length,
        sizeof(length));

    std::memcpy(
        buffer.data() + sizeof(length),
        text.data(),
        text.size());

    PacketReader reader(
        PacketView(
            buffer.data(),
            sizeof(length) + text.size()));

    std::string result;

    EXPECT_TRUE(
        reader.ReadString(result));

    EXPECT_EQ(
        result,
        text);
}

TEST(PacketReaderTests, ReadStringRollbackOnFailure)
{
    std::array<std::byte, 8> buffer{};

    std::uint32_t length = 100;

    std::memcpy(
        buffer.data(),
        &length,
        sizeof(length));

    PacketReader reader(
        PacketView(
            buffer.data(),
            buffer.size()));

    std::string result;

    EXPECT_FALSE(
        reader.ReadString(result));

    EXPECT_EQ(
        reader.GetOffset(),
        0u);
}

TEST(PacketReaderTests, ReadBytesRejectsNullDestination)
{
    std::array<std::byte, 8> buffer{};

    PacketReader reader(
        PacketView(
            buffer.data(),
            buffer.size()));

    EXPECT_FALSE(
        reader.ReadBytes(
            nullptr,
            4));

    EXPECT_EQ(
        reader.GetOffset(),
        0u);
}