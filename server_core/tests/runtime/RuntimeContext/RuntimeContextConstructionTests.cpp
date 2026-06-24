#include <gtest/gtest.h>

#include "mmo/runtime/RuntimeContext/RuntimeContext.hpp"

using namespace mmo::runtime;

TEST(RuntimeContextConstructionTests, DefaultConstruction)
{
    RuntimeContext context;

    EXPECT_EQ(context.tickId, 0u);

    EXPECT_EQ(context.frameIndex, 0u);

    EXPECT_EQ(context.deltaTimeMs, 0u);

    EXPECT_EQ(
        context.mode,
        RuntimeMode::Initializing);

    EXPECT_EQ(
        context.workerCount,
        0u);
}

TEST(RuntimeContextConstructionTests, AggregateInitialization)
{
    RuntimeContext context{
        100,
        200,
        33,
        RuntimeMode::Running,
        8};

    EXPECT_EQ(context.tickId, 100u);

    EXPECT_EQ(context.frameIndex, 200u);

    EXPECT_EQ(context.deltaTimeMs, 33u);

    EXPECT_EQ(
        context.mode,
        RuntimeMode::Running);

    EXPECT_EQ(
        context.workerCount,
        8u);
}