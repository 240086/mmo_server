// #include <gtest/gtest.h>

// #include <mmo/infrastructure/lockfree_queue/MPSCQueue.hpp>

// using namespace mmo::infrastructure::lockfree_queue;

// TEST(MPSCQueueTest, FullQueueRejectsPush)
// {
//     MPSCQueue<int, 4> queue;

//     ASSERT_TRUE(queue.Enqueue(1));
//     ASSERT_TRUE(queue.Enqueue(2));
//     ASSERT_TRUE(queue.Enqueue(3));
//     ASSERT_TRUE(queue.Enqueue(4));

//     ASSERT_FALSE(queue.Enqueue(5));
// }