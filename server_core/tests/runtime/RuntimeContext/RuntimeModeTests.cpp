#include <gtest/gtest.h>

#include "mmo/runtime/RuntimeContext/RuntimeMode.hpp"

using namespace mmo::runtime;

TEST(RuntimeModeTests, EnumValuesShouldRemainStable)
{
    EXPECT_EQ(
        static_cast<std::uint8_t>(RuntimeMode::Initializing),
        0);

    EXPECT_EQ(
        static_cast<std::uint8_t>(RuntimeMode::Running),
        1);

    EXPECT_EQ(
        static_cast<std::uint8_t>(RuntimeMode::Stopping),
        2);

    EXPECT_EQ(
        static_cast<std::uint8_t>(RuntimeMode::Stopped),
        3);
}