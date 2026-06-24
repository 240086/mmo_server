#include <gtest/gtest.h>

#include "mmo/runtime/RuntimePhase/RuntimePhase.hpp"

using namespace mmo::runtime;

TEST(RuntimePhaseConstructionTests, DefaultConstruction)
{
    RuntimePhase phase;

    EXPECT_EQ(
        phase.id,
        RuntimePhaseId::Input);
}

TEST(RuntimePhaseConstructionTests, AggregateInitialization)
{
    RuntimePhase phase{
        RuntimePhaseId::Simulation};

    EXPECT_EQ(
        phase.id,
        RuntimePhaseId::Simulation);
}