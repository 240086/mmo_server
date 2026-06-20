// /home/xj/projects/mmo_server/server_core/tests/runtime/timer/TimerCancellationTests.cpp
#include <gtest/gtest.h> // 👈 替换 <cassert>
#include <mmo/infrastructure/timer/TimerQueue.hpp>

using namespace mmo::infrastructure::timer;

// 1. 使用 TEST 宏，无需自己写 main 函数
TEST(TimerQueueTest, CancellationPreventsExecution)
{
  TimerQueue queue;
  bool executed = false;

  const TimerId id = queue.Schedule(Duration(0), [&]()
                                    { executed = true; });

  // 2. 完美消灭 (void)：直接断言 Cancel 必须返回 true
  EXPECT_TRUE(queue.Cancel(id));

  queue.Tick(Clock::now());

  // 3. 使用 GTest 现代断言
  EXPECT_FALSE(executed);
}