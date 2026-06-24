#include <gtest/gtest.h>

#include <type_traits>

#include "mmo/runtime/RuntimePhase/RuntimePhaseTypes.hpp"

using namespace mmo::runtime;

TEST(RuntimePhaseTypesTests, RuntimePhaseIndexShouldBeUint8)
{
    EXPECT_TRUE(
        (std::is_same_v<
            RuntimePhaseIndex,
            std::uint8_t>));
}

TEST(RuntimePhaseTypesTests, RuntimePhaseCountShouldBeUint8)
{
    EXPECT_TRUE(
        (std::is_same_v<
            RuntimePhaseCount,
            std::uint8_t>));
}