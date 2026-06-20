#include <atomic>
#include <thread>
#include <vector>

#include <gtest/gtest.h>

#include <mmo/infrastructure/timer/TimerQueue.hpp>

using namespace mmo::infrastructure::timer;

TEST(TimerQueueTest, MultiProducerSchedulingIsSafe)
{
    TimerQueue queue;

    constexpr int threadCount = 8;

    constexpr int timersPerThread = 1000;

    std::atomic<int> executedCount = 0;

    std::vector<std::thread> workers;

    for (int t = 0; t < threadCount; ++t)
    {
        workers.emplace_back(
            [&]()
            {
                for (int i = 0; i < timersPerThread; ++i)
                {
                    (void)queue.Schedule(
                        Duration(0),
                        [&]()
                        {
                            executedCount.fetch_add(
                                1,
                                std::memory_order_relaxed);
                        });
                }
            });
    }

    for (auto &worker : workers)
    {
        worker.join();
    }

    queue.Tick(Clock::now());

    EXPECT_EQ(
        executedCount.load(),
        threadCount * timersPerThread);
}