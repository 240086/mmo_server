// /home/xj/projects/mmo_server/server_core/tests/infrastructure/packet/PacketHeaderLayoutTests.cpp
#include <gtest/gtest.h>

#include <type_traits>

#include <mmo/infrastructure/packet/PacketHeader.hpp>

using namespace mmo::infrastructure::packet;

TEST(PacketHeaderLayoutTests, HeaderSizeIsStable)
{
    EXPECT_EQ(
        sizeof(PacketHeader),
        16u);
}

TEST(PacketHeaderLayoutTests, HeaderIsStandardLayout)
{
    EXPECT_TRUE(
        std::is_standard_layout_v<
            PacketHeader>);
}

TEST(PacketHeaderLayoutTests, HeaderIsTriviallyCopyable)
{
    EXPECT_TRUE(
        std::is_trivially_copyable_v<
            PacketHeader>);
}