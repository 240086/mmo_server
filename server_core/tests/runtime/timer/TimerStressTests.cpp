#include <atomic>

#include <gtest/gtest.h>

#include <mmo/infrastructure/timer/TimerQueue.hpp>

using namespace mmo::infrastructure::timer;

TEST(TimerQueueTest, StressTestLargeTimerVolume)
{
    TimerQueue queue;

    constexpr int timerCount = 100000;

    std::atomic<int> executedCount = 0;

    for (int i = 0; i < timerCount; ++i)
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

    queue.Tick(Clock::now());

    EXPECT_EQ(
        executedCount.load(),
        timerCount);
}