#include <gtest/gtest.h>

#include <mmo/runtime/RuntimePhase/RuntimePhaseConstants.hpp>

using namespace mmo::runtime;

TEST(RuntimePipelineConstantsTests, PhaseCountShouldRemainStable)
{
    EXPECT_EQ(
        RuntimePhaseCountV1,
        4);
}