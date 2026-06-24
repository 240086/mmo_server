#include <gtest/gtest.h>

#include <mmo/runtime/RuntimePipeline/RuntimePipeline.hpp>

using namespace mmo::runtime;

TEST(RuntimePipelineInitializationTests, ShouldInitializeInputPhase)
{
    RuntimePipeline pipeline;

    EXPECT_EQ(
        pipeline.phases[0].id,
        RuntimePhaseId::Input);
}

TEST(RuntimePipelineInitializationTests, ShouldInitializeSimulationPhase)
{
    RuntimePipeline pipeline;

    EXPECT_EQ(
        pipeline.phases[1].id,
        RuntimePhaseId::Simulation);
}

TEST(RuntimePipelineInitializationTests, ShouldInitializePostSimulationPhase)
{
    RuntimePipeline pipeline;

    EXPECT_EQ(
        pipeline.phases[2].id,
        RuntimePhaseId::PostSimulation);
}

TEST(RuntimePipelineInitializationTests, ShouldInitializeOutputPhase)
{
    RuntimePipeline pipeline;

    EXPECT_EQ(
        pipeline.phases[3].id,
        RuntimePhaseId::Output);
}

TEST(RuntimePipelineInitializationTests, PhaseOrderingMustRemainDeterministic)
{
    RuntimePipeline pipeline;

    EXPECT_EQ(
        pipeline.phases[0].id,
        RuntimePhaseId::Input);

    EXPECT_EQ(
        pipeline.phases[1].id,
        RuntimePhaseId::Simulation);

    EXPECT_EQ(
        pipeline.phases[2].id,
        RuntimePhaseId::PostSimulation);

    EXPECT_EQ(
        pipeline.phases[3].id,
        RuntimePhaseId::Output);
}