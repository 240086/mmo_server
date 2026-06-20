#include <atomic>
#include <thread>

#include <gtest/gtest.h>

#include <mmo/infrastructure/timer/TimerQueue.hpp>

using namespace mmo::infrastructure::timer;

TEST(TimerQueueTest, TimerExecutesNearDeadline)
{
    TimerQueue queue;

    std::atomic<bool> executed = false;

    const auto start = Clock::now();

    (void)queue.Schedule(
        Duration(50),
        [&]()
        {
            executed.store(true);
        });

    while (!executed.load())
    {
        queue.Tick(Clock::now());

        std::this_thread::sleep_for(
            std::chrono::milliseconds(1));
    }

    const auto elapsed =
        std::chrono::duration_cast<Duration>(
            Clock::now() - start);

    EXPECT_GE(elapsed.count(), 45);
}