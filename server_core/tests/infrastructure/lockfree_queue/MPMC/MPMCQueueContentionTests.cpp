#include <gtest/gtest.h>
#include <atomic>
#include <thread>
#include <vector>
#include <mmo/infrastructure/lockfree_queue/MPMCQueue.hpp>

using namespace mmo::infrastructure::lockfree_queue;

namespace
{

    TEST(MPMCQueueContentionTests, HeavyMPMCContentionWithQualityCheck)
    {
        constexpr int Producers = 8;
        constexpr int Consumers = 8;
        constexpr int Count = 100000;

        MPMCQueue<int, 65536> queue;

        std::atomic<int> produced{0};
        std::atomic<int> consumed{0};

        // 🎯 终极加固：建立原子值域验证桶，大小等于 Count
        std::vector<std::atomic<int>> delivery_buckets(Count);
        for (auto &bucket : delivery_buckets)
        {
            bucket.store(0, std::memory_order_relaxed);
        }

        std::vector<std::thread> threads;

        // 生产者线程组
        for (int i = 0; i < Producers; ++i)
        {
            threads.emplace_back(
                [&]()
                {
                    for (int n = 0; n < Count; ++n)
                    {
                        while (!queue.Enqueue(n))
                        {
                            std::this_thread::yield(); // 避免过度榨干测试机
                        }

                        produced.fetch_add(1, std::memory_order_relaxed);
                    }
                });
        }

        // 消费者线程组
        for (int i = 0; i < Consumers; ++i)
        {
            threads.emplace_back(
                [&]()
                {
                    int value;
                    while (consumed.load(std::memory_order_relaxed) < Producers * Count)
                    {
                        if (queue.Dequeue(value))
                        {
                            // 🎯 穿透校验：验证值域合法性并落桶计数
                            if (value >= 0 && value < Count)
                            {
                                delivery_buckets[value].fetch_add(1, std::memory_order_relaxed);
                            }

                            consumed.fetch_add(1, std::memory_order_relaxed);
                        }
                        else
                        {
                            std::this_thread::yield(); // 缓解测试后期的 CPU 盲自旋
                        }
                    }
                });
        }

        for (auto &t : threads)
        {
            t.join();
        }

        // 1. 数量断言 (Quantity)
        EXPECT_EQ(produced.load(), Producers * Count);
        EXPECT_EQ(consumed.load(), Producers * Count);

        // 2. 质量断言 (Quality)：确保每一个数字都被精确消费了 Producers 次，无漏读、无重读
        for (int i = 0; i < Count; ++i)
        {
            ASSERT_EQ(delivery_buckets[i].load(), Producers)
                << "💥 Data corruption detected! Value [" << i
                << "] was consumed " << delivery_buckets[i].load() << " times, expected " << Producers;
        }
    }

}