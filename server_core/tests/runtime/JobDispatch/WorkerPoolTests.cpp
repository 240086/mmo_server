#include "JobDispatchTestCommon.hpp"

#include "mmo/runtime/JobDispatch/WorkerPool.hpp"
#include <mmo/runtime/JobDispatch/ITaskExecutor.hpp>
#include <mmo/runtime/JobDispatch/ITaskCompletionSink.hpp>

namespace mmo::runtime::test
{

    class MockExecutor final
        : public ITaskExecutor
    {
    public:
        std::atomic<uint32_t> executed{0};

        void ExecuteTask(TaskId) noexcept override
        {
            executed.fetch_add(1);
        }
    };

    class MockCompletionSink final
        : public ITaskCompletionSink
    {
    public:
        std::atomic<uint32_t> completed{0};

        void OnTaskCompleted(TaskId) noexcept override
        {
            completed.fetch_add(1);
        }
    };

    TEST(WorkerPoolTests, StartReturnsFalseForInvalidConfig)
    {
        WorkerPool pool;

        WorkerPoolConfig cfg;

        EXPECT_FALSE(
            pool.Start(cfg));
    }

    TEST(WorkerPoolTests, StartAndStopLifecycle)
    {
        WorkerPool pool;

        MockExecutor executor;
        MockCompletionSink sink;

        mmo::infrastructure::lockfree_queue::
            MPMCQueue<TaskId, TaskGraphMaxNodesV1>
                queue;

        WorkerPoolConfig cfg;

        cfg.executor = &executor;
        cfg.completionSink = &sink;
        cfg.readyQueue = &queue;

        EXPECT_TRUE(
            pool.Start(cfg));

        EXPECT_TRUE(
            pool.Running());

        pool.Stop();
        pool.Join();

        EXPECT_FALSE(
            pool.Running());
    }

}