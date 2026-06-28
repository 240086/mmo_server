// /tests/runtime/RuntimePipelineExecutor/RuntimePipelineExecutorExecutionTests.cpp

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "RuntimePipelineExecutorTestCommon.hpp"

namespace mmo::runtime::testing
{

    class RuntimePipelineExecutorExecutionTests
        : public RuntimePipelineExecutorFixture
    {
    protected:
        void PrepareInitializedExecutor(
            RuntimePipelineExecutor &executor)
        {
            EXPECT_CALL(
                jobDispatch,
                Initialize(
                    ::testing::Ref(taskGraph),
                    ::testing::Ref(runtimeContext),
                    ::testing::Ref(taskRegistry)))
                .WillOnce(::testing::Return(true));

            ASSERT_TRUE(executor.Initialize());
        }
    };

    TEST_F(
        RuntimePipelineExecutorExecutionTests,
        ExecutePipelineExecutesAllPhasesInOrder)
    {
        auto executor = CreateExecutor();

        PrepareInitializedExecutor(executor);

        {
            ::testing::InSequence sequence;

            EXPECT_CALL(
                jobDispatch,
                ExecutePhase(RuntimePhaseId::Input))
                .WillOnce(::testing::Return(true));

            EXPECT_CALL(
                jobDispatch,
                ExecutePhase(RuntimePhaseId::Simulation))
                .WillOnce(::testing::Return(true));

            EXPECT_CALL(
                jobDispatch,
                ExecutePhase(RuntimePhaseId::PostSimulation))
                .WillOnce(::testing::Return(true));

            EXPECT_CALL(
                jobDispatch,
                ExecutePhase(RuntimePhaseId::Output))
                .WillOnce(::testing::Return(true));
        }

        EXPECT_EQ(
            executor.ExecutePipeline(),
            RuntimeExecutionResult::Success);
    }

    TEST_F(
        RuntimePipelineExecutorExecutionTests,
        ExecutePipelineReturnsSuccess)
    {
        auto executor = CreateExecutor();

        PrepareInitializedExecutor(executor);

        EXPECT_CALL(
            jobDispatch,
            ExecutePhase(::testing::_))
            .Times(4)
            .WillRepeatedly(::testing::Return(true));

        EXPECT_EQ(
            executor.ExecutePipeline(),
            RuntimeExecutionResult::Success);
    }

    TEST_F(
        RuntimePipelineExecutorExecutionTests,
        ExecutePipelineRestoresInitializedState)
    {
        auto executor = CreateExecutor();

        PrepareInitializedExecutor(executor);

        EXPECT_CALL(
            jobDispatch,
            ExecutePhase(::testing::_))
            .Times(4)
            .WillRepeatedly(::testing::Return(true));

        EXPECT_EQ(
            executor.ExecutePipeline(),
            RuntimeExecutionResult::Success);

        EXPECT_EQ(
            executor.GetState(),
            RuntimePipelineExecutorState::Initialized);
    }

    TEST_F(
        RuntimePipelineExecutorExecutionTests,
        ExecutePipelineDispatchesExactlyFourPhases)
    {
        auto executor = CreateExecutor();

        PrepareInitializedExecutor(executor);

        EXPECT_CALL(
            jobDispatch,
            ExecutePhase(::testing::_))
            .Times(4)
            .WillRepeatedly(::testing::Return(true));

        EXPECT_EQ(
            executor.ExecutePipeline(),
            RuntimeExecutionResult::Success);
    }

    TEST_F(
        RuntimePipelineExecutorExecutionTests,
        ExecutePipelineNeverCallsShutdown)
    {
        auto executor = CreateExecutor();

        PrepareInitializedExecutor(executor);

        EXPECT_CALL(
            jobDispatch,
            Shutdown())
            .Times(0);

        EXPECT_CALL(
            jobDispatch,
            ExecutePhase(::testing::_))
            .Times(4)
            .WillRepeatedly(::testing::Return(true));

        EXPECT_EQ(
            executor.ExecutePipeline(),
            RuntimeExecutionResult::Success);
    }

    TEST_F(
        RuntimePipelineExecutorExecutionTests,
        ExecutePipelineCanRunMultipleTicks)
    {
        auto executor = CreateExecutor();

        PrepareInitializedExecutor(executor);

        EXPECT_CALL(
            jobDispatch,
            ExecutePhase(::testing::_))
            .Times(12)
            .WillRepeatedly(::testing::Return(true));

        EXPECT_EQ(
            executor.ExecutePipeline(),
            RuntimeExecutionResult::Success);

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
        RuntimePipelineExecutorExecutionTests,
        ExecutePipelineDoesNotReinitializeDispatcher)
    {
        auto executor = CreateExecutor();

        EXPECT_CALL(
            jobDispatch,
            Initialize(
                ::testing::Ref(taskGraph),
                ::testing::Ref(runtimeContext),
                ::testing::Ref(taskRegistry)))
            .Times(1)
            .WillOnce(::testing::Return(true));

        ASSERT_TRUE(executor.Initialize());

        EXPECT_CALL(
            jobDispatch,
            ExecutePhase(::testing::_))
            .Times(4)
            .WillRepeatedly(::testing::Return(true));

        EXPECT_EQ(
            executor.ExecutePipeline(),
            RuntimeExecutionResult::Success);
    }

} // namespace mmo::runtime::testing