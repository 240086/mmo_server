// #include <gtest/gtest.h>

// #include <mmo/infrastructure/lockfree_queue/MPSCQueue.hpp>

// using namespace mmo::infrastructure::lockfree_queue;

// TEST(MPSCQueueTest, SequenceWrapAroundWorks)
// {
//     MPSCQueue<int, 8> queue;

//     constexpr int kIterations = 100000;

//     for (int i = 0; i < kIterations; ++i)
//     {
//         while (!queue.Enqueue(i))
//         {
//         }

//         int value = -1;

//         ASSERT_TRUE(queue.Dequeue(value));

//         ASSERT_EQ(value, i);
//     }
// }