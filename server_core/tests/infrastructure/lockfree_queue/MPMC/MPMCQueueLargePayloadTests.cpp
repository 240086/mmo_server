#include <gtest/gtest.h>

#include <atomic>
#include <thread>
#include <vector>

#include <mmo/infrastructure/lockfree_queue/MPMCQueue.hpp>
using namespace mmo::infrastructure::lockfree_queue;

namespace
{
    struct Payload
    {
        std::array<std::uint8_t, 1024> data{};
    };

    TEST(MPMCQueueLargePayloadTests, LargeObjectTransfer)
    {
        MPMCQueue<Payload, 64> queue;

        Payload payload;

        payload.data[777] = 123;

        EXPECT_TRUE(queue.Enqueue(payload));

        Payload result;

        EXPECT_TRUE(queue.Dequeue(result));

        EXPECT_EQ(
            result.data[777],
            123);
    }
}