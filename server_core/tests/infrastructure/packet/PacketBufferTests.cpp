// server_core/tests/infrastructure/packet/PacketBufferTests.cpp

#include <gtest/gtest.h>

#include <utility>

#include <mmo/infrastructure/packet/PacketBuffer.hpp>
#include <mmo/infrastructure/packet/PacketConstants.hpp>

using namespace mmo::infrastructure::packet;

TEST(PacketBufferTests, DefaultConstruction)
{
    PacketBuffer buffer;

    EXPECT_EQ(buffer.Size(), 0u);
    EXPECT_TRUE(buffer.Empty());

    EXPECT_GE(
        buffer.Capacity(),
        kDefaultPacketCapacity);
}

TEST(PacketBufferTests, CustomCapacityConstruction)
{
    constexpr std::size_t capacity = 8192;

    PacketBuffer buffer(capacity);

    EXPECT_EQ(buffer.Size(), 0u);
    EXPECT_TRUE(buffer.Empty());

    EXPECT_GE(
        buffer.Capacity(),
        capacity);
}

TEST(PacketBufferTests, ConstructionRejectsOversizedCapacity)
{
    EXPECT_THROW(
        PacketBuffer(kMaxPacketSize + 1),
        std::length_error);
}

TEST(PacketBufferTests, ResizeGrowsBuffer)
{
    PacketBuffer buffer;

    buffer.Resize(1024);

    EXPECT_EQ(
        buffer.Size(),
        1024u);

    EXPECT_FALSE(
        buffer.Empty());
}

TEST(PacketBufferTests, ResizeShrinksBuffer)
{
    PacketBuffer buffer;

    buffer.Resize(4096);

    EXPECT_EQ(
        buffer.Size(),
        4096u);

    buffer.Resize(128);

    EXPECT_EQ(
        buffer.Size(),
        128u);
}

TEST(PacketBufferTests, ResizeRejectsOversizedBuffer)
{
    PacketBuffer buffer;

    EXPECT_THROW(
        buffer.Resize(kMaxPacketSize + 1),
        std::length_error);
}

TEST(PacketBufferTests, ReserveIncreasesCapacity)
{
    PacketBuffer buffer;

    constexpr std::size_t capacity = 8192;

    buffer.Reserve(capacity);

    EXPECT_GE(
        buffer.Capacity(),
        capacity);
}

TEST(PacketBufferTests, ReserveRejectsOversizedCapacity)
{
    PacketBuffer buffer;

    EXPECT_THROW(
        buffer.Reserve(kMaxPacketSize + 1),
        std::length_error);
}

TEST(PacketBufferTests, ClearResetsSizeOnly)
{
    PacketBuffer buffer;

    buffer.Reserve(8192);

    const auto originalCapacity =
        buffer.Capacity();

    buffer.Resize(4096);

    EXPECT_EQ(
        buffer.Size(),
        4096u);

    buffer.Clear();

    EXPECT_EQ(
        buffer.Size(),
        0u);

    EXPECT_TRUE(
        buffer.Empty());

    EXPECT_EQ(
        buffer.Capacity(),
        originalCapacity);
}

TEST(PacketBufferTests, DataPointerIsValidAfterResize)
{
    PacketBuffer buffer;

    buffer.Resize(128);

    EXPECT_NE(
        buffer.Data(),
        nullptr);
}

TEST(PacketBufferTests, ConstDataPointerIsValidAfterResize)
{
    PacketBuffer buffer;

    buffer.Resize(128);

    const PacketBuffer &constBuffer = buffer;

    EXPECT_NE(
        constBuffer.Data(),
        nullptr);
}

TEST(PacketBufferTests, MoveConstructionTransfersBuffer)
{
    PacketBuffer source;

    source.Resize(1024);

    auto *originalData =
        source.Data();

    PacketBuffer moved(
        std::move(source));

    EXPECT_EQ(
        moved.Size(),
        1024u);

    EXPECT_EQ(
        moved.Data(),
        originalData);
}

TEST(PacketBufferTests, MoveAssignmentTransfersBuffer)
{
    PacketBuffer source;

    source.Resize(2048);

    auto *originalData =
        source.Data();

    PacketBuffer target;

    target = std::move(source);

    EXPECT_EQ(
        target.Size(),
        2048u);

    EXPECT_EQ(
        target.Data(),
        originalData);
}