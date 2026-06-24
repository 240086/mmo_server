#include <gtest/gtest.h>

#include "mmo/runtime/RuntimePhase/RuntimePhaseId.hpp"

using namespace mmo::runtime;

TEST(RuntimePhaseIdTests, EnumValuesShouldRemainStable)
{
    EXPECT_EQ(
        static_cast<std::uint8_t>(
            RuntimePhaseId::Input),
        0);

    EXPECT_EQ(
        static_cast<std::uint8_t>(
            RuntimePhaseId::Simulation),
        1);

    EXPECT_EQ(
        static_cast<std::uint8_t>(
            RuntimePhaseId::PostSimulation),
        2);

    EXPECT_EQ(
        static_cast<std::uint8_t>(
            RuntimePhaseId::Output),
        3);
}