#include <gtest/gtest.h>

#include <mmo/infrastructure/lockfree_queue/MPMCQueue.hpp>

using namespace mmo::infrastructure::lockfree_queue;

namespace
{

    TEST(MPMCQueueBasicTests, SingleEnqueueDequeue)
    {
        MPMCQueue<int, 8> queue;

        EXPECT_TRUE(queue.Enqueue(42));

        int value = 0;

        EXPECT_TRUE(queue.Dequeue(value));

        EXPECT_EQ(value, 42);
    }

    TEST(MPMCQueueBasicTests, EmptyQueueReturnsFalse)
    {
        MPMCQueue<int, 8> queue;

        int value = 0;

        EXPECT_FALSE(queue.Dequeue(value));
    }

}