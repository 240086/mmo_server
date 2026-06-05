// /home/xj/projects/mmo_server/server_core/tests/infrastructure/packet/PacketViewTests.cpp
#include <gtest/gtest.h>

#include <array>

#include <mmo/infrastructure/packet/PacketView.hpp>

using namespace mmo::infrastructure::packet;

TEST(PacketViewTests, EmptyViewConstruction)
{
    PacketView view(nullptr, 0);

    EXPECT_TRUE(view.Empty());

    EXPECT_EQ(view.Size(), 0u);

    EXPECT_EQ(view.Data(), nullptr);
}

TEST(PacketViewTests, NonEmptyViewConstruction)
{
    std::array<std::byte, 16> buffer{};

    PacketView view(
        buffer.data(),
        buffer.size());

    EXPECT_FALSE(view.Empty());

    EXPECT_EQ(
        view.Size(),
        buffer.size());

    EXPECT_EQ(
        view.Data(),
        buffer.data());
}

TEST(PacketViewTests, DataReturnsOriginalPointer)
{
    std::array<std::byte, 32> buffer{};

    PacketView view(
        buffer.data(),
        buffer.size());

    EXPECT_EQ(
        view.Data(),
        buffer.data());
}

TEST(PacketViewTests, SizeReturnsOriginalSize)
{
    std::array<std::byte, 64> buffer{};

    PacketView view(
        buffer.data(),
        buffer.size());

    EXPECT_EQ(
        view.Size(),
        64u);
}

TEST(PacketViewTests, FullSubViewReturnsEquivalentView)
{
    std::array<std::byte, 32> buffer{};

    PacketView view(
        buffer.data(),
        buffer.size());

    PacketView sub =
        view.SubView(0, 32);

    EXPECT_EQ(
        sub.Data(),
        buffer.data());

    EXPECT_EQ(
        sub.Size(),
        32u);
}

TEST(PacketViewTests, MiddleSubViewReturnsCorrectSlice)
{
    std::array<std::byte, 64> buffer{};

    PacketView view(
        buffer.data(),
        buffer.size());

    PacketView sub =
        view.SubView(16, 24);

    EXPECT_EQ(
        sub.Data(),
        buffer.data() + 16);

    EXPECT_EQ(
        sub.Size(),
        24u);
}

TEST(PacketViewTests, TailSubViewReturnsCorrectSlice)
{
    std::array<std::byte, 64> buffer{};

    PacketView view(
        buffer.data(),
        buffer.size());

    PacketView sub =
        view.SubView(48, 16);

    EXPECT_EQ(
        sub.Data(),
        buffer.data() + 48);

    EXPECT_EQ(
        sub.Size(),
        16u);
}

TEST(PacketViewTests, ZeroLengthSubViewIsValid)
{
    std::array<std::byte, 64> buffer{};

    PacketView view(
        buffer.data(),
        buffer.size());

    PacketView sub =
        view.SubView(16, 0);

    EXPECT_EQ(
        sub.Data(),
        buffer.data() + 16);

    EXPECT_EQ(
        sub.Size(),
        0u);

    EXPECT_TRUE(
        sub.Empty());
}

TEST(PacketViewTests, OffsetPastEndReturnsEmptyView)
{
    std::array<std::byte, 64> buffer{};

    PacketView view(
        buffer.data(),
        buffer.size());

    PacketView sub =
        view.SubView(128, 8);

    EXPECT_TRUE(
        sub.Empty());

    EXPECT_EQ(
        sub.Size(),
        0u);

    EXPECT_EQ(
        sub.Data(),
        nullptr);
}

TEST(PacketViewTests, RangePastEndReturnsEmptyView)
{
    std::array<std::byte, 64> buffer{};

    PacketView view(
        buffer.data(),
        buffer.size());

    PacketView sub =
        view.SubView(60, 8);

    EXPECT_TRUE(
        sub.Empty());

    EXPECT_EQ(
        sub.Size(),
        0u);

    EXPECT_EQ(
        sub.Data(),
        nullptr);
}

TEST(PacketViewTests, NestedSubViewWorksCorrectly)
{
    std::array<std::byte, 128> buffer{};

    PacketView root(
        buffer.data(),
        buffer.size());

    PacketView first =
        root.SubView(32, 64);

    PacketView second =
        first.SubView(16, 24);

    EXPECT_EQ(
        second.Data(),
        buffer.data() + 48);

    EXPECT_EQ(
        second.Size(),
        24u);
}

TEST(PacketViewTests, EmptyViewCanCreateEmptySubView)
{
    PacketView view(nullptr, 0);

    PacketView sub =
        view.SubView(0, 0);

    EXPECT_TRUE(
        sub.Empty());

    EXPECT_EQ(
        sub.Size(),
        0u);
}