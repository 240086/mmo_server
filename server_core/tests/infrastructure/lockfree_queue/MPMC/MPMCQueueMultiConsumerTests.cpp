#include <gtest/gtest.h>
#include <atomic>
#include <thread>
#include <vector>
#include <mmo/infrastructure/lockfree_queue/MPMCQueue.hpp>

using namespace mmo::infrastructure::lockfree_queue;

namespace
{

    TEST(MPMCQueueMultiConsumerTests, AllItemsConsumedPreventStarvation)
    {
        constexpr int Count = 50000;
        MPMCQueue<int, 65536> queue;

        for (int i = 0; i < Count; ++i)
        {
            EXPECT_TRUE(queue.Enqueue(i));
        }

        std::atomic<int> consumed{0};
        std::vector<std::thread> workers;

        for (int i = 0; i < 8; ++i)
        {
            workers.emplace_back(
                [&]()
                {
                    int value;
                    while (consumed.load(std::memory_order_relaxed) < Count)
                    {
                        if (queue.Dequeue(value))
                        {
                            consumed.fetch_add(1, std::memory_order_relaxed);
                        }
                        else
                        {
                            // 🎯 优化核心：让出时间片，防止 8 个 Worker 在队列变空时强行把物理核心拉满
                            std::this_thread::yield();
                        }
                    }
                });
        }

        for (auto &t : workers)
        {
            t.join();
        }

        EXPECT_EQ(consumed.load(), Count);
    }

}