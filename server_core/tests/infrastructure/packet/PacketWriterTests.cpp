// /home/xj/projects/mmo_server/server_core/tests/infrastructure/packet/PacketWriterTests.cpp
#include <gtest/gtest.h>

#include <array>
#include <cstdint>
#include <string>
#include <vector>
#include <cstring>

#include <mmo/infrastructure/packet/PacketBuffer.hpp>
#include <mmo/infrastructure/packet/PacketWriter.hpp>

using namespace mmo::infrastructure::packet;

TEST(PacketWriterTests, InitialOffsetIsZero)
{
    PacketBuffer buffer;

    PacketWriter writer(buffer);

    EXPECT_EQ(
        writer.GetOffset(),
        0u);
}

TEST(PacketWriterTests, WriteUInt32)
{
    PacketBuffer buffer;

    PacketWriter writer(buffer);

    std::uint32_t value = 123456;

    EXPECT_TRUE(
        writer.Write(value));

    EXPECT_EQ(
        writer.GetOffset(),
        sizeof(value));

    EXPECT_EQ(
        buffer.Size(),
        sizeof(value));

    std::uint32_t stored = 0;

    std::memcpy(
        &stored,
        buffer.Data(),
        sizeof(stored));

    EXPECT_EQ(
        stored,
        value);
}

TEST(PacketWriterTests, WriteBytes)
{
    PacketBuffer buffer;

    PacketWriter writer(buffer);

    std::array<std::byte, 8> data{};

    EXPECT_TRUE(
        writer.WriteBytes(
            data.data(),
            data.size()));

    EXPECT_EQ(
        writer.GetOffset(),
        data.size());
}

TEST(PacketWriterTests, RejectsNullPointer)
{
    PacketBuffer buffer;

    PacketWriter writer(buffer);

    EXPECT_FALSE(
        writer.WriteBytes(
            nullptr,
            8));

    EXPECT_EQ(
        writer.GetOffset(),
        0u);
}

TEST(PacketWriterTests, AcceptsZeroSizeWrite)
{
    PacketBuffer buffer;

    PacketWriter writer(buffer);

    EXPECT_TRUE(
        writer.WriteBytes(
            nullptr,
            0));

    EXPECT_EQ(
        writer.GetOffset(),
        0u);
}

TEST(PacketWriterTests, AutomaticallyResizesBuffer)
{
    PacketBuffer buffer(16);

    PacketWriter writer(buffer);

    std::array<std::byte, 128> payload{};

    EXPECT_TRUE(
        writer.WriteBytes(
            payload.data(),
            payload.size()));

    EXPECT_EQ(
        buffer.Size(),
        payload.size());
}

TEST(PacketWriterTests, RejectsOversizedWrite)
{
    PacketBuffer buffer;

    PacketWriter writer(buffer);

    // 提示：确保 kMaxPacketSize 能够被测试正确识别，如果它定义在 mmo 命名空间下可直接使用
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

TEST(PacketWriterTests, WriteString)
{
    PacketBuffer buffer;

    PacketWriter writer(buffer);

    EXPECT_TRUE(
        writer.WriteString(
            "MMO"));

    EXPECT_EQ(
        writer.GetOffset(),
        sizeof(std::uint32_t) + 3);
}

TEST(PacketWriterTests, WriteEmptyString)
{
    PacketBuffer buffer;

    PacketWriter writer(buffer);

    EXPECT_TRUE(
        writer.WriteString(""));
}

TEST(PacketWriterTests, ResetOnlyResetsCursor)
{
    PacketBuffer buffer;

    PacketWriter writer(buffer);

    std::uint32_t value = 42;

    // 修改点：使用 EXPECT_TRUE 消除 [[nodiscard]] 警告，并确保前置写入状态正确
    EXPECT_TRUE(
        writer.Write(value));

    writer.Reset();

    EXPECT_EQ(
        writer.GetOffset(),
        0u);

    EXPECT_EQ(
        buffer.Size(),
        sizeof(value));
}

TEST(PacketWriterTests, WriteSpan)
{
    PacketBuffer buffer;

    PacketWriter writer(buffer);

    std::array<std::byte, 32> payload{};

    EXPECT_TRUE(
        writer.WriteSpan(
            payload.data(),
            payload.size()));

    EXPECT_EQ(
        writer.GetOffset(),
        payload.size());
}