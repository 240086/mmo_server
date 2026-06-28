#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "RuntimePipelineExecutorTestCommon.hpp"

using namespace mmo::runtime;
namespace mmo::runtime::testing
{

    class RuntimePipelineExecutorConstructionTests
        : public RuntimePipelineExecutorFixture
    {
    };

    TEST_F(RuntimePipelineExecutorConstructionTests, DefaultConstruction_SetsValidState)
    {
        auto executor = CreateExecutor();
        EXPECT_EQ(
            executor.GetState(),
            RuntimePipelineExecutorState::Created);
    }

    TEST_F(RuntimePipelineExecutorConstructionTests, Config_DefaultValues_AreCorrect)
    {
        RuntimePipelineExecutorConfig config{};

        EXPECT_TRUE(config.failFast);
        EXPECT_FALSE(config.enableProfiling);
        EXPECT_FALSE(config.collectMetrics);
    }

    TEST_F(RuntimePipelineExecutorConstructionTests, Construction_DoesNotCallJobDispatch)
    {
        auto executor = CreateExecutor();
        EXPECT_CALL(jobDispatch, Initialize)
            .Times(0);

        EXPECT_CALL(jobDispatch, ExecutePhase)
            .Times(0);

        EXPECT_CALL(jobDispatch, Shutdown)
            .Times(0);

        EXPECT_EQ(
            executor.GetState(),
            RuntimePipelineExecutorState::Created);
    }

    TEST_F(RuntimePipelineExecutorConstructionTests, Construction_AllDependenciesAreBound)
    {
        // 只验证不会 crash / invalid access
        // 不验证行为（属于 integration）

        EXPECT_NO_THROW(
            {
                RuntimePipelineExecutor local(
                    RuntimePipelineExecutorConfig{},
                    pipeline,
                    runtimeContext,
                    taskGraph,
                    taskRegistry,
                    jobDispatch);

                EXPECT_EQ(
                    local.GetState(),
                    RuntimePipelineExecutorState::Created);
            });
    }

    TEST_F(RuntimePipelineExecutorConstructionTests, MultipleInstances_AreIndependent)
    {
        RuntimePipelineExecutor a(
            RuntimePipelineExecutorConfig{},
            pipeline,
            runtimeContext,
            taskGraph,
            taskRegistry,
            jobDispatch);

        RuntimePipelineExecutor b(
            RuntimePipelineExecutorConfig{},
            pipeline,
            runtimeContext,
            taskGraph,
            taskRegistry,
            jobDispatch);

        EXPECT_EQ(a.GetState(), RuntimePipelineExecutorState::Created);
        EXPECT_EQ(b.GetState(), RuntimePipelineExecutorState::Created);
    }
}