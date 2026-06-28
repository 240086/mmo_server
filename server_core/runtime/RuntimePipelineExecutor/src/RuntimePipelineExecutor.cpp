// /home/xj/projects/mmo_server/server_core/runtime/RuntimePipelineExecutor/src/RuntimePipelineExecutor.cpp
#include "mmo/runtime/RuntimePipelineExecutor/RuntimePipelineExecutor.hpp"

#include <stdexcept>

#include "mmo/runtime/JobDispatch/IJobDispatch.hpp"
#include "mmo/runtime/RuntimeContext/RuntimeContext.hpp"
#include "mmo/runtime/RuntimePhase/RuntimePhase.hpp"
#include "mmo/runtime/RuntimePipeline/RuntimePipeline.hpp"

namespace mmo::runtime
{

    RuntimePipelineExecutor::RuntimePipelineExecutor(
        RuntimePipelineExecutorConfig config,
        RuntimePipeline &pipeline,
        RuntimeContext &runtimeContext,
        const TaskGraph &taskGraph,
        const TaskRegistry &taskRegistry,
        IJobDispatch &jobDispatch)
        : m_config(std::move(config)),
          m_pipeline(pipeline),
          m_runtimeContext(runtimeContext),
          m_taskGraph(taskGraph),
          m_taskRegistry(taskRegistry),
          m_jobDispatch(jobDispatch)
    {
    }

    bool RuntimePipelineExecutor::Initialize()
    {
        if (m_state != RuntimePipelineExecutorState::Created)
        {
            throw std::logic_error(
                "RuntimePipelineExecutor already initialized");
        }

        // 强内聚初始化：在此处将真理源对象下沉注入 JobDispatch，激活底层物理线程池与 CSR 拓扑
        if (!m_jobDispatch.Initialize(
                m_taskGraph,
                m_runtimeContext,
                m_taskRegistry))
        {
            return false;
        }

        m_state =
            RuntimePipelineExecutorState::Initialized;

        return true;
    }

    void RuntimePipelineExecutor::Shutdown()
    {
        if (m_state ==
            RuntimePipelineExecutorState::Stopped)
        {
            return;
        }

        m_state =
            RuntimePipelineExecutorState::Stopping;

        // 向下传递生命周期，强行排空并关闭物理工作线程池
        m_jobDispatch.Shutdown();
        m_state =
            RuntimePipelineExecutorState::Stopped;
    }

    RuntimeExecutionResult
    RuntimePipelineExecutor::ExecutePipeline()
    {
        if (m_state ==
            RuntimePipelineExecutorState::Created)
        {
            throw std::logic_error(
                "ExecutePipeline before Initialize");
        }

        if (m_state ==
            RuntimePipelineExecutorState::Stopped)
        {
            throw std::logic_error(
                "ExecutePipeline after Shutdown");
        }

        m_state =
            RuntimePipelineExecutorState::Running;

        // 使用隐藏了具体存储结构的迭代器进行无感知安全遍历
        for (RuntimePhase &phase : m_pipeline)
        {
            const auto result =
                ExecutePhase(phase);

            if (result !=
                RuntimeExecutionResult::Success)
            {
                // 异常 Fail-Fast 退出分支：必须强制恢复至 Initialized 态以允许下一次 Tick 尝试
                m_state =
                    RuntimePipelineExecutorState::Initialized;
                return result;
            }
        }

        // 单次 Tick 管线完整驱动完毕，顺利收敛状态机，等待下一次节拍触发
        m_state =
            RuntimePipelineExecutorState::Initialized;

        return RuntimeExecutionResult::Success;
    }

    RuntimeExecutionResult
    RuntimePipelineExecutor::ExecutePhase(
        RuntimePhase &phase)
    {
        // 控制面语义完美转换为物理调度面：在此处向 JobDispatch 提交并等待当前 Phase 的 Barrier 撞墙同步
        if (!m_jobDispatch.ExecutePhase(phase.id))
        {
            return RuntimeExecutionResult::DispatchFailure;
        }

        //
        // V1
        //
        // RuntimePhase currently provides
        // topology only.
        //
        // TaskGraph submission will be added
        // after RuntimePipelineExecutor V1
        // becomes stable.
        //

        return RuntimeExecutionResult::Success;
    }

    RuntimePipelineExecutorState
    RuntimePipelineExecutor::GetState() const noexcept
    {
        return m_state;
    }

} // namespace mmo::runtime