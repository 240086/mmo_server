// /home/xj/projects/mmo_server/server_core/tests/runtime/RuntimePipelineExecutor/RuntimePipelineExecutorTestCommon.hpp
#pragma once

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include <mmo/runtime/JobDispatch/IJobDispatch.hpp>
#include <mmo/runtime/JobDispatch/TaskRegistry.hpp>
#include <mmo/runtime/RuntimeContext/RuntimeContext.hpp>
#include <mmo/runtime/RuntimePhase/RuntimePhaseId.hpp>
#include <mmo/runtime/RuntimePipeline/RuntimePipeline.hpp>
#include <mmo/runtime/RuntimePipelineExecutor/RuntimePipelineExecutor.hpp>
#include <mmo/runtime/RuntimePipelineExecutor/RuntimePipelineExecutorConfig.hpp>
#include <mmo/runtime/TaskGraph/TaskGraph.hpp>

namespace mmo::runtime::testing
{
    class MockJobDispatch 
        : public IJobDispatch
    {
    public:
        MOCK_METHOD(
            bool,
            Initialize,
            (const TaskGraph &, RuntimeContext &, const TaskRegistry &),
            (override));

        MOCK_METHOD(
            bool,
            ExecutePhase,
            (RuntimePhaseId),
            (override));

        MOCK_METHOD(
            void,
            Shutdown,
            (),
            (override));
    };

    class RuntimePipelineExecutorFixture
        : public ::testing::Test
    {
    protected:
        RuntimePipelineExecutorFixture() = default;
        ~RuntimePipelineExecutorFixture() override = default;

        // 🎯 工业级测试工厂：利用 C++20 RVO 机制，完美绕过重型组件的 =delete 移动构造限制
        [[nodiscard]]
        RuntimePipelineExecutor CreateExecutor(
            RuntimePipelineExecutorConfig customConfig = {})
        {
            return RuntimePipelineExecutor{
                customConfig,
                pipeline,
                runtimeContext,
                taskGraph,
                taskRegistry,
                static_cast<IJobDispatch &>(jobDispatch)};
        }

    protected:
        // 🧩 环境依赖上下文资产（严格控制声明顺序）
        RuntimePipeline pipeline;
        RuntimeContext runtimeContext;
        TaskGraph taskGraph;
        TaskRegistry taskRegistry;

        // 🛡️ 严格模拟存根（全量监控未预期调用）
        ::testing::StrictMock<MockJobDispatch> jobDispatch;
    };
} // namespace mmo::runtime::testing