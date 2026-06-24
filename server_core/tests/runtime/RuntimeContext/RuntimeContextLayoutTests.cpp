#include <gtest/gtest.h>

#include <type_traits>

#include "mmo/runtime/RuntimeContext/RuntimeContext.hpp"

using namespace mmo::runtime;

TEST(RuntimeContextLayoutTests, ShouldBeStandardLayout)
{
    EXPECT_TRUE(
        std::is_standard_layout_v<RuntimeContext>);
}

TEST(RuntimeContextLayoutTests, ShouldBeTriviallyCopyable)
{
    EXPECT_TRUE(
        std::is_trivially_copyable_v<RuntimeContext>);
}

TEST(RuntimeContextLayoutTests, ShouldBeTriviallyDestructible)
{
    EXPECT_TRUE(
        std::is_trivially_destructible_v<RuntimeContext>);
}

TEST(RuntimeContextLayoutTests, ShouldNotContainVirtualTable)
{
    EXPECT_FALSE(
        std::has_virtual_destructor_v<RuntimeContext>);
}

TEST(RuntimeContextLayoutTests, SizeShouldRemainStable)
{
    constexpr std::size_t expectedMaxSize = 32;

    EXPECT_LE(
        sizeof(RuntimeContext),
        expectedMaxSize);
}