// /home/xj/projects/mmo_server/server_core/runtime/JobDispatch/include/mmo/runtime/JobDispatch/JobDispatch.hpp
#pragma once

#include <array>
#include <atomic>

#include "mmo/infrastructure/lockfree_queue/MPMCQueue.hpp"
#include "mmo/runtime/JobDispatch/IJobDispatch.hpp"
#include "mmo/runtime/JobDispatch/ITaskCompletionSink.hpp"
#include "mmo/runtime/JobDispatch/ITaskExecutor.hpp"
#include "mmo/runtime/JobDispatch/JobDispatchConstants.hpp"
#include "mmo/runtime/JobDispatch/JobDispatchTypes.hpp"
#include "mmo/runtime/JobDispatch/TaskRegistry.hpp"
#include "mmo/runtime/JobDispatch/TaskRuntimeState.hpp"
#include "mmo/runtime/JobDispatch/WorkerPool.hpp"

namespace mmo::runtime
{

    class TaskGraph;
    class RuntimeContext;

    /**
     * @brief Layer2 核心拓扑执行调度引擎 (Single Source of Truth)
     * @note [FINAL_FROZEN_V1] 唯一拥有拓扑、队列及势能状态的中央权威。
     */
    class JobDispatch final
        : public IJobDispatch,
          public ITaskCompletionSink,
          public ITaskExecutor
    {
    public:
        JobDispatch() = default;
        ~JobDispatch() override = default;

        JobDispatch(const JobDispatch &) = delete;
        JobDispatch &operator=(const JobDispatch &) = delete;
        JobDispatch(JobDispatch &&) = delete;
        JobDispatch &operator=(JobDispatch &&) = delete;

    public:
        bool Initialize(
            const TaskGraph &graph,
            RuntimeContext &context,
            const TaskRegistry &registry) = 0;
        bool ExecutePhase(RuntimePhaseId phaseId) override;
        void Shutdown() override;

    public:
        void OnTaskCompleted(TaskId taskId) noexcept override;
        void ExecuteTask(TaskId taskId) noexcept override;

    private:
        void BuildRuntimeState();
        void SeedReadyQueue();
        void ReleaseSuccessors(TaskId completedTask);

        /**
         * @brief 基于 Generation Epoch 的高可靠屏障
         * * @note 【CPP IMPLEMENTATION STRICT PROTOCOL / C++ 实现严控协议】
         * 为防止双原子变量（activeCount & epoch）由于观测顺序造成的 Lost Wakeup，
         * cpp 的实现必须严格遵循以下范式：
         * * [主线程 WaitForPhaseCompletion]:
         * while (m_activeTaskCount.load(std::memory_order_acquire) != 0) {
         * auto currentEpoch = m_phaseEpoch.load(std::memory_order_acquire);
         * // Double-check 防御
         * if (m_activeTaskCount.load(std::memory_order_acquire) == 0) break;
         * m_phaseEpoch.wait(currentEpoch, std::memory_order_relaxed);
         * }
         * * [工作线程 OnTaskCompleted - 拓扑归零释放处]:
         * uint32_t prevCount = m_activeTaskCount.fetch_sub(1, std::memory_order_acq_rel);
         * if (prevCount == 1) { // 我是最后一个完成的任务
         * m_phaseEpoch.fetch_add(1, std::memory_order_release);
         * m_phaseEpoch.notify_all();
         * }
         */
        void WaitForPhaseCompletion();

    private:
        // ============================================================================
        // 外部生命周期绑定 (Outlive Contract)
        // ============================================================================
        const TaskGraph *m_taskGraph{nullptr};
        RuntimeContext *m_runtimeContext{nullptr};

        // ============================================================================
        // 核心物理执行与调度资源
        // ============================================================================
        WorkerPool m_workerPool;
        const TaskRegistry *m_registry{nullptr};

        mmo::infrastructure::lockfree_queue::MPMCQueue<TaskId, TaskGraphMaxNodesV1> m_readyQueue;
        std::array<TaskRuntimeState, TaskGraphMaxNodesV1> m_runtimeState;

        // ============================================================================
        // 并发同步状态机 (Generation Barrier & Lifecycle)
        // ============================================================================
        alignas(kCacheLineSize) std::atomic<std::uint32_t> m_activeTaskCount{0};
        alignas(kCacheLineSize) std::atomic<std::uint64_t> m_phaseEpoch{0};

        // 消除 m_shutdown 重叠语义，唯一持有模块运行生命周期
        std::atomic<bool> m_running{false};
    };

} // namespace mmo::runtime