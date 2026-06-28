#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "RuntimePipelineExecutorTestCommon.hpp"

namespace mmo::runtime::testing
{

    class RuntimePipelineExecutorLifecycleTests
        : public RuntimePipelineExecutorFixture
    {
    };

    TEST_F(
        RuntimePipelineExecutorLifecycleTests,
        InitializeSuccessTransitionsToInitialized)
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

        EXPECT_EQ(
            executor.GetState(),
            RuntimePipelineExecutorState::Initialized);
    }

    TEST_F(
        RuntimePipelineExecutorLifecycleTests,
        InitializeFailureKeepsCreatedState)
    {
        auto executor = CreateExecutor();

        EXPECT_CALL(
            jobDispatch,
            Initialize(
                ::testing::Ref(taskGraph),
                ::testing::Ref(runtimeContext),
                ::testing::Ref(taskRegistry)))
            .WillOnce(::testing::Return(false));

        EXPECT_FALSE(executor.Initialize());

        EXPECT_EQ(
            executor.GetState(),
            RuntimePipelineExecutorState::Created);
    }

    TEST_F(
        RuntimePipelineExecutorLifecycleTests,
        InitializeTwiceThrowsLogicError)
    {
        auto executor = CreateExecutor();

        EXPECT_CALL(jobDispatch, Initialize)
            .WillOnce(::testing::Return(true));

        EXPECT_TRUE(executor.Initialize());

        EXPECT_THROW(
            executor.Initialize(),
            std::logic_error);
    }

    TEST_F(
        RuntimePipelineExecutorLifecycleTests,
        ShutdownBeforeInitializeTransitionsToStopped)
    {
        auto executor = CreateExecutor();

        EXPECT_CALL(jobDispatch, Shutdown)
            .Times(1);

        executor.Shutdown();

        EXPECT_EQ(
            executor.GetState(),
            RuntimePipelineExecutorState::Stopped);
    }

    TEST_F(
        RuntimePipelineExecutorLifecycleTests,
        ShutdownAfterInitializeCallsJobDispatch)
    {
        auto executor = CreateExecutor();

        {
            ::testing::InSequence seq;

            EXPECT_CALL(jobDispatch, Initialize)
                .WillOnce(::testing::Return(true));

            EXPECT_CALL(jobDispatch, Shutdown);
        }

        ASSERT_TRUE(executor.Initialize());

        executor.Shutdown();

        EXPECT_EQ(
            executor.GetState(),
            RuntimePipelineExecutorState::Stopped);
    }

    TEST_F(
        RuntimePipelineExecutorLifecycleTests,
        ShutdownTwiceIsIdempotent)
    {
        auto executor = CreateExecutor();

        EXPECT_CALL(jobDispatch, Shutdown)
            .Times(1);

        executor.Shutdown();

        executor.Shutdown();

        EXPECT_EQ(
            executor.GetState(),
            RuntimePipelineExecutorState::Stopped);
    }

    TEST_F(
        RuntimePipelineExecutorLifecycleTests,
        LifecycleOrderIsStrict)
    {
        auto executor = CreateExecutor();

        {
            ::testing::InSequence seq;

            EXPECT_CALL(jobDispatch, Initialize)
                .WillOnce(::testing::Return(true));

            EXPECT_CALL(jobDispatch, Shutdown);
        }

        ASSERT_TRUE(executor.Initialize());

        EXPECT_EQ(
            executor.GetState(),
            RuntimePipelineExecutorState::Initialized);

        executor.Shutdown();

        EXPECT_EQ(
            executor.GetState(),
            RuntimePipelineExecutorState::Stopped);
    }

} // namespace mmo::runtime::testing