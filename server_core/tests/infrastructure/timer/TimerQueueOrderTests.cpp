// /home/xj/projects/mmo_server/server_core/tests/runtime/timer/TimerQueueOrderTests.cpp
#include <gtest/gtest.h>
#include <vector>
#include <mmo/infrastructure/timer/TimerQueue.hpp>

using namespace mmo::infrastructure::timer;

TEST(TimerQueueTest, TasksWithSameDelayExecuteInFIFOOrder)
{
  TimerQueue queue;
  std::vector<int> executionOrder;

  // 这里因为不需要用到 TimerId，依然可以用 (void) 丢弃返回值
  (void)queue.Schedule(Duration(0), [&]()
                       { executionOrder.push_back(1); });
  (void)queue.Schedule(Duration(0), [&]()
                       { executionOrder.push_back(2); });
  (void)queue.Schedule(Duration(0), [&]()
                       { executionOrder.push_back(3); });

  queue.Tick(Clock::now());

  // 现代断言：更易读，且失败时会打印具体不相等的数值
  ASSERT_EQ(executionOrder.size(), 3U); // ASSERT_EQ 失败会直接终止当前 CASE，防止后面越界
  EXPECT_EQ(executionOrder[0], 1);
  EXPECT_EQ(executionOrder[1], 2);
  EXPECT_EQ(executionOrder[2], 3);
}