#include "mmo/runtime/JobDispatch/WorkerPool.hpp"

#include <thread>

#include "mmo/runtime/JobDispatch/ITaskCompletionSink.hpp"
#include "mmo/runtime/JobDispatch/ITaskExecutor.hpp"

namespace mmo::runtime
{

    bool WorkerPool::Start(
        const WorkerPoolConfig &config)
    {
        if (Running())
        {
            return false;
        }

        if (
            config.executor == nullptr ||
            config.completionSink == nullptr ||
            config.readyQueue == nullptr)
        {
            return false;
        }

        m_config = config;

        m_activeWorkerCount =
            static_cast<std::uint16_t>(
                std::thread::hardware_concurrency());

        if (
            m_activeWorkerCount == 0 ||
            m_activeWorkerCount > kMaxHardwareWorkers)
        {
            m_activeWorkerCount = 1;
        }

        m_running.store(
            true,
            std::memory_order_release);

        for (
            WorkerIndex i = 0;
            i < m_activeWorkerCount;
            ++i)
        {
            m_contexts[i].workerIndex = i;

            m_threads[i].workerIndex = i;
            m_threads[i].executedTasks = 0;
            m_threads[i].stealCount = 0;

            m_threads[i].thread =
                std::thread(
                    &WorkerPool::WorkerLoop,
                    this,
                    i);
        }

        return true;
    }

    void WorkerPool::Stop()
    {
        m_running.store(
            false,
            std::memory_order_release);
    }

    void WorkerPool::Join()
    {
        for (
            std::uint16_t i = 0;
            i < m_activeWorkerCount;
            ++i)
        {
            if (m_threads[i].thread.joinable())
            {
                m_threads[i].thread.join();
            }
        }

        m_activeWorkerCount = 0;
    }

    bool WorkerPool::Running() const noexcept
    {
        return m_running.load(
            std::memory_order_acquire);
    }

    void WorkerPool::WorkerLoop(
        WorkerIndex)
    {
        TaskId taskId;

        while (
            m_running.load(
                std::memory_order_acquire))
        {
            if (!m_config.readyQueue->Dequeue(taskId))
            {
                std::this_thread::yield();
                continue;
            }

            m_config.executor
                ->ExecuteTask(taskId);

            m_config.completionSink
                ->OnTaskCompleted(taskId);
        }
    }

}