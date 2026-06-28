// /home/xj/projects/mmo_server/server_core/runtime/RuntimePipelineExecutor/include/mmo/runtime/RuntimePipelineExecutor/RuntimePipelineExecutor.hpp
#pragma once

#include <mmo/runtime/RuntimePipelineExecutor/IRuntimePipelineExecutor.hpp>
#include <mmo/runtime/RuntimePipelineExecutor/RuntimePipelineExecutorConfig.hpp>

namespace mmo::runtime
{
    class RuntimePipeline;
    class RuntimeContext;
    class IJobDispatch;
    class RuntimePhase;
    class TaskGraph;
    class TaskRegistry;

    class RuntimePipelineExecutor final
        : public IRuntimePipelineExecutor
    {
    public:
        /**
         * Ownership
         *
         * RuntimePipeline owns all phases.
         *
         * RuntimePipelineExecutor never owns
         * RuntimePhase objects.
         */
        RuntimePipelineExecutor(
            RuntimePipelineExecutorConfig config,
            RuntimePipeline &pipeline,
            RuntimeContext &runtimeContext,
            const TaskGraph &taskGraph,
            const TaskRegistry &taskRegistry,
            IJobDispatch &jobDispatch);

        ~RuntimePipelineExecutor() override = default;

        RuntimePipelineExecutor(
            const RuntimePipelineExecutor &) = delete;

        RuntimePipelineExecutor &
        operator=(
            const RuntimePipelineExecutor &) = delete;

        RuntimePipelineExecutor(
            RuntimePipelineExecutor &&) = delete;

        RuntimePipelineExecutor &
        operator=(
            const RuntimePipelineExecutor &&) = delete;

    public:
        bool Initialize() override;

        void Shutdown() override;

        RuntimeExecutionResult ExecutePipeline() override;

        [[nodiscard]]
        RuntimePipelineExecutorState
        GetState() const noexcept override;

    private:
        RuntimeExecutionResult
        ExecutePhase(
            RuntimePhase &phase);

    private:
        RuntimePipelineExecutorConfig m_config;

        RuntimePipeline &m_pipeline;

        RuntimeContext &m_runtimeContext;

        const TaskGraph &m_taskGraph;

        const TaskRegistry &m_taskRegistry;

        IJobDispatch &m_jobDispatch;

        RuntimePipelineExecutorState m_state{
            RuntimePipelineExecutorState::Created};
    };

} // namespace mmo::runtime