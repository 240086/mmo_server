#include <mmo/runtime/JobDispatch/JobDispatch.hpp>
#include <mmo/runtime/TaskGraph/TaskGraph.hpp>
#include <mmo/runtime/TaskGraph/TaskGraph.hpp>
#include <cassert>

namespace mmo::runtime
{
    bool JobDispatch::Initialize(
        const TaskGraph &graph,
        RuntimeContext &context,
        const TaskRegistry &registry)
    {
        if (m_running.load())
        {
            return false;
        }

        if (!registry.Validate(graph))
        {
            return false;
        }

        m_taskGraph = &graph;
        m_runtimeContext = &context;
        m_registry = &registry;

        WorkerPoolConfig config;
        config.executor = this;
        config.completionSink = this;
        config.readyQueue = &m_readyQueue;

        if (!m_workerPool.Start(config))
        {
            return false;
        }

        m_running.store(
            true,
            std::memory_order_release);

        return true;
    }

    void JobDispatch::ExecuteTask(TaskId taskId) noexcept
    {
        const TaskFunction fn = m_registry->Resolve(taskId);

        if (fn == nullptr)
        {
            return;
        }

        fn(*m_runtimeContext);
    }

    void JobDispatch::BuildRuntimeState()
    {
        for (
            TaskNodeCount i = 0;
            i < m_taskGraph->nodeCount;
            ++i)
        {
            const TaskNode &node = m_taskGraph->nodes[i];

            m_runtimeState[node.id]
                .remainingDependencies
                .store(
                    node.parentCount,
                    std::memory_order_relaxed);
        }
    }

    void JobDispatch::SeedReadyQueue()
    {
        // 获取当前阶段 (Acquire)
        const auto currentPhase = m_currentPhase.load(std::memory_order_acquire);
        const auto &range =
            m_taskGraph->phaseRanges[static_cast<std::size_t>(currentPhase)];

        for (
            std::uint16_t i = 0;
            i < range.count;
            ++i)
        {
            const TaskNode &node =
                m_taskGraph->nodes[range.begin + i];

            if (
                m_runtimeState[node.id]
                    .remainingDependencies
                    .load(std::memory_order_relaxed) == 0)
            {
                const bool ok = m_readyQueue.Enqueue(node.id);
                assert(ok && "FATAL: ReadyQueue capacity exceeded during SeedReadyQueue!");
                (void)ok; // 消除 Release 模式下的未使用警告

                m_activeTaskCount.fetch_add(1, std::memory_order_relaxed);
            }
        }
    }

    void JobDispatch::ReleaseSuccessors(TaskId completedTask)
    {
        // FIX 1: 使用安全的 GetNode 获取当前节点
        const TaskNode &node = GetNode(completedTask);

        // Worker 线程获取当前阶段边界 (Acquire)，防止跨越到未来阶段
        const auto currentPhase = m_currentPhase.load(std::memory_order_acquire);

        for (std::uint32_t i = 0; i < node.childCount; ++i)
        {
            const TaskId child = m_taskGraph->childTaskIds[node.dependencyOffset + i];
            auto &state = m_runtimeState[child];

            const auto old = state.remainingDependencies.fetch_sub(1, std::memory_order_acq_rel);

            if (old == 1)
            {
                // FIX 1: 使用安全的 GetNode 获取子节点
                const TaskNode &childNode = GetNode(child);

                if (childNode.phaseId == currentPhase)
                {
                    // FIX 2: MPMCQueue 容量防线
                    const bool ok = m_readyQueue.Enqueue(child);
                    assert(ok && "FATAL: ReadyQueue capacity exceeded during ReleaseSuccessors!");
                    (void)ok;

                    m_activeTaskCount.fetch_add(1, std::memory_order_relaxed);
                }
            }
        }
    }

    void JobDispatch::OnTaskCompleted(TaskId taskId) noexcept
    {
        ReleaseSuccessors(taskId);

        const auto prev =
            m_activeTaskCount.fetch_sub(
                1,
                std::memory_order_acq_rel);

        if (prev == 1)
        {
            m_phaseEpoch.fetch_add(
                1,
                std::memory_order_release);

            m_phaseEpoch.notify_all();
        }
    }

    void JobDispatch::WaitForPhaseCompletion()
    {
        while (
            m_activeTaskCount.load(
                std::memory_order_acquire) != 0)
        {
            const auto epoch =
                m_phaseEpoch.load(
                    std::memory_order_acquire);

            if (
                m_activeTaskCount.load(
                    std::memory_order_acquire) == 0)
            {
                break;
            }

            m_phaseEpoch.wait(
                epoch,
                std::memory_order_relaxed);
        }
    }

    bool JobDispatch::ExecutePhase(RuntimePhaseId phaseId)
    {
        if (!m_running.load(
                std::memory_order_acquire))
        {
            return false;
        }

        // 主线程发布 (Release) 新阶段状态，确保此前所有内存操作对 Worker 可见
        m_currentPhase.store(phaseId, std::memory_order_release);

        if (phaseId == RuntimePhaseId::Input)
        {
            BuildRuntimeState();
        }

        SeedReadyQueue();
        WaitForPhaseCompletion();

        return true;
    }

    void JobDispatch::Shutdown()
    {
        if (!m_running.exchange(false))
        {
            return;
        }

        m_workerPool.Stop();
        m_workerPool.Join();

        // FIX 3: 彻底清空所有运行时势能与状态机，保证可重入性
        m_activeTaskCount.store(0, std::memory_order_relaxed);
        m_phaseEpoch.store(0, std::memory_order_relaxed);

        // 重置为初始状态
        m_currentPhase.store(RuntimePhaseId::Input, std::memory_order_release);

        for (auto &state : m_runtimeState)
        {
            state.remainingDependencies.store(0, std::memory_order_relaxed);
        }

        m_taskGraph = nullptr;
        m_runtimeContext = nullptr;
        m_registry = nullptr;
    }

    const TaskNode &JobDispatch::GetNode(TaskId id) const noexcept
    {
        const auto index =
            m_taskGraph->taskIdToNodeIndex[id];

        return m_taskGraph->nodes[index];
    }
}