// #include <gtest/gtest.h>

// #include <mmo/infrastructure/lockfree_queue/MPSCQueue.hpp>

// using namespace mmo::infrastructure::lockfree_queue;

// TEST(MPSCQueueTest, SingleProducerFIFOOrder)
// {
//     MPSCQueue<int, 1024> queue;

//     for (int i = 0; i < 1000; ++i)
//     {
//         ASSERT_TRUE(queue.Enqueue(i));
//     }

//     for (int i = 0; i < 1000; ++i)
//     {
//         int value = -1;

//         ASSERT_TRUE(queue.Dequeue(value));

//         ASSERT_EQ(value, i);
//     }
// }