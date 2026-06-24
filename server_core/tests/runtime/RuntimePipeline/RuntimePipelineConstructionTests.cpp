#include <gtest/gtest.h>

#include <mmo/runtime/RuntimePipeline/RuntimePipeline.hpp>

using namespace mmo::runtime;

TEST(RuntimePipelineConstructionTests, DefaultConstruction)
{
    RuntimePipeline pipeline;

    EXPECT_EQ(
        pipeline.phases.size(),
        RuntimePhaseCountV1);
}

TEST(RuntimePipelineConstructionTests, AggregateInitialization)
{
    RuntimePipeline pipeline{};

    EXPECT_EQ(
        pipeline.phases.size(),
        RuntimePhaseCountV1);
}