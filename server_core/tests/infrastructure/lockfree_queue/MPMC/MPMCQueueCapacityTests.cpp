#include <gtest/gtest.h>

#include <mmo/infrastructure/lockfree_queue/MPMCQueue.hpp>

using namespace mmo::infrastructure::lockfree_queue;

namespace
{

    TEST(MPMCQueueCapacityTests, FullQueueRejectsPush)
    {
        MPMCQueue<int, 4> queue;

        EXPECT_TRUE(queue.Enqueue(1));
        EXPECT_TRUE(queue.Enqueue(2));
        EXPECT_TRUE(queue.Enqueue(3));
        EXPECT_TRUE(queue.Enqueue(4));

        EXPECT_FALSE(queue.Enqueue(5));
    }

}