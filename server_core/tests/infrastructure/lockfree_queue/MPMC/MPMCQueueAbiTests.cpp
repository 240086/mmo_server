#include <gtest/gtest.h>

#include <type_traits>

#include <mmo/infrastructure/lockfree_queue/MPMCQueue.hpp>

using namespace mmo::infrastructure::lockfree_queue;

namespace
{

    TEST(MPMCQueueAbiTests, QueueSlotContract)
    {
        static_assert(
            alignof(QueueSlot<std::uint32_t>) ==
            kCacheLineSize);

        static_assert(
            std::is_standard_layout_v<
                QueueSlot<std::uint32_t>>);

        static_assert(
            std::is_trivially_destructible_v<
                QueueSlot<std::uint32_t>>);
    }

    TEST(MPMCQueueAbiTests, QueueConstruction)
    {
        MPMCQueue<int, 256> queue;

        EXPECT_TRUE(queue.Empty());
        EXPECT_EQ(queue.CapacityValue(), 256u);
        EXPECT_EQ(queue.ApproxSize(), 0u);
    }

}