#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "RuntimePipelineExecutorTestCommon.hpp"

namespace mmo::runtime::testing
{

    class RuntimePipelineExecutorIntegrationTests
        : public RuntimePipelineExecutorFixture
    {
    };

    TEST_F(
        RuntimePipelineExecutorIntegrationTests,
        InitializeUsesCorrectDependencies)
    {
        auto executor = CreateExecutor();

        EXPECT_CALL(
            jobDispatch,
            Initialize(
                ::testing::Ref(taskGraph),
                ::testing::Ref(runtimeContext),
                ::testing::Ref(taskRegistry)))
            .WillOnce(::testing::Return(true));

        EXPECT_TRUE(executor.Initialize());
    }

    TEST_F(
        RuntimePipelineExecutorIntegrationTests,
        ExecutePipelineVisitsEveryRuntimePhase)
    {
        auto executor = CreateExecutor();

        {
            ::testing::InSequence seq;

            EXPECT_CALL(jobDispatch, Initialize)
                .WillOnce(::testing::Return(true));

            EXPECT_CALL(jobDispatch, ExecutePhase(RuntimePhaseId::Input))
                .WillOnce(::testing::Return(true));

            EXPECT_CALL(jobDispatch, ExecutePhase(RuntimePhaseId::Simulation))
                .WillOnce(::testing::Return(true));

            EXPECT_CALL(jobDispatch, ExecutePhase(RuntimePhaseId::PostSimulation))
                .WillOnce(::testing::Return(true));

            EXPECT_CALL(jobDispatch, ExecutePhase(RuntimePhaseId::Output))
                .WillOnce(::testing::Return(true));
        }

        ASSERT_TRUE(executor.Initialize());

        EXPECT_EQ(
            executor.ExecutePipeline(),
            RuntimeExecutionResult::Success);
    }

    TEST_F(
        RuntimePipelineExecutorIntegrationTests,
        ExecutePipelineNeverReordersRuntimePhases)
    {
        auto executor = CreateExecutor();

        ::testing::InSequence sequence;

        EXPECT_CALL(jobDispatch, Initialize)
            .WillOnce(::testing::Return(true));

        EXPECT_CALL(jobDispatch, ExecutePhase(RuntimePhaseId::Input))
            .WillOnce(::testing::Return(true));

        EXPECT_CALL(jobDispatch, ExecutePhase(RuntimePhaseId::Simulation))
            .WillOnce(::testing::Return(true));

        EXPECT_CALL(jobDispatch, ExecutePhase(RuntimePhaseId::PostSimulation))
            .WillOnce(::testing::Return(true));

        EXPECT_CALL(jobDispatch, ExecutePhase(RuntimePhaseId::Output))
            .WillOnce(::testing::Return(true));

        ASSERT_TRUE(executor.Initialize());

        EXPECT_EQ(
            executor.ExecutePipeline(),
            RuntimeExecutionResult::Success);
    }

    TEST_F(
        RuntimePipelineExecutorIntegrationTests,
        MultipleTicksRemainStable)
    {
        auto executor = CreateExecutor();

        EXPECT_CALL(jobDispatch, Initialize)
            .WillOnce(::testing::Return(true));

        ASSERT_TRUE(executor.Initialize());

        EXPECT_CALL(jobDispatch, ExecutePhase(::testing::_))
            .Times(8)
            .WillRepeatedly(::testing::Return(true));

        EXPECT_EQ(
            executor.ExecutePipeline(),
            RuntimeExecutionResult::Success);

        EXPECT_EQ(
            executor.ExecutePipeline(),
            RuntimeExecutionResult::Success);

        EXPECT_EQ(
            executor.GetState(),
            RuntimePipelineExecutorState::Initialized);
    }

    TEST_F(
        RuntimePipelineExecutorIntegrationTests,
        ShutdownAfterMultipleTicks)
    {
        auto executor = CreateExecutor();

        {
            ::testing::InSequence seq;

            EXPECT_CALL(jobDispatch, Initialize)
                .WillOnce(::testing::Return(true));

            EXPECT_CALL(jobDispatch, ExecutePhase(::testing::_))
                .Times(8)
                .WillRepeatedly(::testing::Return(true));

            EXPECT_CALL(jobDispatch, Shutdown);
        }

        ASSERT_TRUE(executor.Initialize());

        EXPECT_EQ(
            executor.ExecutePipeline(),
            RuntimeExecutionResult::Success);

        EXPECT_EQ(
            executor.ExecutePipeline(),
            RuntimeExecutionResult::Success);

        executor.Shutdown();

        EXPECT_EQ(
            executor.GetState(),
            RuntimePipelineExecutorState::Stopped);
    }

    TEST_F(
        RuntimePipelineExecutorIntegrationTests,
        FullLifecycleIntegration)
    {
        auto executor = CreateExecutor();

        {
            ::testing::InSequence seq;

            EXPECT_CALL(jobDispatch, Initialize)
                .WillOnce(::testing::Return(true));

            EXPECT_CALL(jobDispatch, ExecutePhase(RuntimePhaseId::Input))
                .WillOnce(::testing::Return(true));

            EXPECT_CALL(jobDispatch, ExecutePhase(RuntimePhaseId::Simulation))
                .WillOnce(::testing::Return(true));

            EXPECT_CALL(jobDispatch, ExecutePhase(RuntimePhaseId::PostSimulation))
                .WillOnce(::testing::Return(true));

            EXPECT_CALL(jobDispatch, ExecutePhase(RuntimePhaseId::Output))
                .WillOnce(::testing::Return(true));

            EXPECT_CALL(jobDispatch, Shutdown);
        }

        ASSERT_TRUE(executor.Initialize());

        EXPECT_EQ(
            executor.ExecutePipeline(),
            RuntimeExecutionResult::Success);

        executor.Shutdown();

        EXPECT_EQ(
            executor.GetState(),
            RuntimePipelineExecutorState::Stopped);
    }

} // namespace mmo::runtime::testing