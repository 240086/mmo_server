#include <gtest/gtest.h>

#include <type_traits>

#include "mmo/runtime/RuntimePhase/RuntimePhase.hpp"

using namespace mmo::runtime;

TEST(RuntimePhaseLayoutTests, ShouldBeStandardLayout)
{
    EXPECT_TRUE(
        std::is_standard_layout_v<RuntimePhase>);
}

TEST(RuntimePhaseLayoutTests, ShouldBeTriviallyCopyable)
{
    EXPECT_TRUE(
        std::is_trivially_copyable_v<RuntimePhase>);
}

TEST(RuntimePhaseLayoutTests, ShouldBeTriviallyDestructible)
{
    EXPECT_TRUE(
        std::is_trivially_destructible_v<RuntimePhase>);
}

TEST(RuntimePhaseLayoutTests, ShouldNotContainVirtualTable)
{
    EXPECT_FALSE(
        std::is_polymorphic_v<RuntimePhase>);
}

TEST(RuntimePhaseLayoutTests, SizeShouldRemainStable)
{
    EXPECT_LE(
        sizeof(RuntimePhase),
        sizeof(std::uint8_t));
}