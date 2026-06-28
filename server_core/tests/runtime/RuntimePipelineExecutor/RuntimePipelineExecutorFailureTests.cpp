#include <stdexcept>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "RuntimePipelineExecutorTestCommon.hpp"

namespace mmo::runtime::testing
{

    class RuntimePipelineExecutorFailureTests
        : public RuntimePipelineExecutorFixture
    {
    };

    TEST_F(
        RuntimePipelineExecutorFailureTests,
        ExecutePipelineBeforeInitializeThrowsLogicError)
    {
        auto executor = CreateExecutor();

        EXPECT_THROW(
            executor.ExecutePipeline(),
            std::logic_error);
    }

    TEST_F(
        RuntimePipelineExecutorFailureTests,
        ExecutePipelineAfterShutdownThrowsLogicError)
    {
        auto executor = CreateExecutor();

        EXPECT_CALL(jobDispatch, Shutdown);

        executor.Shutdown();

        EXPECT_THROW(
            executor.ExecutePipeline(),
            std::logic_error);
    }

    TEST_F(
        RuntimePipelineExecutorFailureTests,
        ExecutePipelineReturnsDispatchFailureWhenFirstPhaseFails)
    {
        auto executor = CreateExecutor();

        EXPECT_CALL(jobDispatch, Initialize)
            .WillOnce(::testing::Return(true));

        ASSERT_TRUE(executor.Initialize());

        EXPECT_CALL(
            jobDispatch,
            ExecutePhase(RuntimePhaseId::Input))
            .WillOnce(::testing::Return(false));

        EXPECT_EQ(
            executor.ExecutePipeline(),
            RuntimeExecutionResult::DispatchFailure);

        EXPECT_EQ(
            executor.GetState(),
            RuntimePipelineExecutorState::Initialized);
    }

    TEST_F(
        RuntimePipelineExecutorFailureTests,
        ExecutePipelineStopsImmediatelyAfterFailure)
    {
        auto executor = CreateExecutor();

        EXPECT_CALL(jobDispatch, Initialize)
            .WillOnce(::testing::Return(true));

        ASSERT_TRUE(executor.Initialize());

        {
            ::testing::InSequence seq;

            EXPECT_CALL(
                jobDispatch,
                ExecutePhase(RuntimePhaseId::Input))
                .WillOnce(::testing::Return(true));

            EXPECT_CALL(
                jobDispatch,
                ExecutePhase(RuntimePhaseId::Simulation))
                .WillOnce(::testing::Return(false));
        }

        EXPECT_CALL(
            jobDispatch,
            ExecutePhase(RuntimePhaseId::PostSimulation))
            .Times(0);

        EXPECT_CALL(
            jobDispatch,
            ExecutePhase(RuntimePhaseId::Output))
            .Times(0);

        EXPECT_EQ(
            executor.ExecutePipeline(),
            RuntimeExecutionResult::DispatchFailure);

        EXPECT_EQ(
            executor.GetState(),
            RuntimePipelineExecutorState::Initialized);
    }

    TEST_F(
        RuntimePipelineExecutorFailureTests,
        ExecutorCanExecuteAgainAfterDispatchFailure)
    {
        auto executor = CreateExecutor();

        EXPECT_CALL(jobDispatch, Initialize)
            .WillOnce(::testing::Return(true));

        ASSERT_TRUE(executor.Initialize());

        {
            ::testing::InSequence seq;

            EXPECT_CALL(jobDispatch, ExecutePhase(RuntimePhaseId::Input))
                .WillOnce(::testing::Return(false));

            EXPECT_CALL(jobDispatch, ExecutePhase(RuntimePhaseId::Input))
                .WillOnce(::testing::Return(true));

            EXPECT_CALL(jobDispatch, ExecutePhase(RuntimePhaseId::Simulation))
                .WillOnce(::testing::Return(true));

            EXPECT_CALL(jobDispatch, ExecutePhase(RuntimePhaseId::PostSimulation))
                .WillOnce(::testing::Return(true));

            EXPECT_CALL(jobDispatch, ExecutePhase(RuntimePhaseId::Output))
                .WillOnce(::testing::Return(true));
        }

        EXPECT_EQ(
            executor.ExecutePipeline(),
            RuntimeExecutionResult::DispatchFailure);

        EXPECT_EQ(
            executor.GetState(),
            RuntimePipelineExecutorState::Initialized);

        EXPECT_EQ(
            executor.ExecutePipeline(),
            RuntimeExecutionResult::Success);

        EXPECT_EQ(
            executor.GetState(),
            RuntimePipelineExecutorState::Initialized);
    }

    TEST_F(
        RuntimePipelineExecutorFailureTests,
        InitializeFailureAllowsRetry)
    {
        auto executor = CreateExecutor();

        {
            ::testing::InSequence seq;

            EXPECT_CALL(jobDispatch, Initialize)
                .WillOnce(::testing::Return(false));

            EXPECT_CALL(jobDispatch, Initialize)
                .WillOnce(::testing::Return(true));
        }

        EXPECT_FALSE(executor.Initialize());

        EXPECT_EQ(
            executor.GetState(),
            RuntimePipelineExecutorState::Created);

        EXPECT_TRUE(executor.Initialize());

        EXPECT_EQ(
            executor.GetState(),
            RuntimePipelineExecutorState::Initialized);
    }

} // namespace mmo::runtime::testing