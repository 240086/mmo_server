#include <gtest/gtest.h>
#include <atomic>
#include <thread>
#include <vector>
#include <memory> // 🎯 引入智能指针
#include <mmo/infrastructure/lockfree_queue/MPMCQueue.hpp>

using namespace mmo::infrastructure::lockfree_queue;

namespace
{

    TEST(MPMCQueueMultiProducerTests, AllItemsDelivered)
    {
        constexpr int ProducerCount = 8;
        constexpr int ItemsPerProducer = 10000;
        constexpr int TotalItems = ProducerCount * ItemsPerProducer; // 80000

        // 🎯 终极修复：使用 std::make_unique 将 8MB+ 的重型无锁队列强行转移至【堆(Heap)空间】
        // 从而使当前测试函数的栈帧开销骤降至 8 字节（仅剩一个指针），彻底免疫 TSan 爆栈
        auto queue = std::make_unique<MPMCQueue<int, 131072>>();

        std::atomic<int> produced{0};
        std::vector<std::thread> threads;

        for (int p = 0; p < ProducerCount; ++p)
        {
            threads.emplace_back(
                [&]()
                {
                    for (int i = 0; i < ItemsPerProducer; ++i)
                    {
                        // 使用指针形式调用
                        while (!queue->Enqueue(i))
                        {
                            std::this_thread::yield();
                        }

                        produced.fetch_add(1, std::memory_order_relaxed);
                    }
                });
        }

        for (auto &t : threads)
        {
            t.join();
        }

        // 验证生产数量
        EXPECT_EQ(produced.load(), TotalItems);

        // 抽干验证，确保全量交付
        int dummy = 0;
        int consume_count = 0;
        while (queue->Dequeue(dummy))
        {
            consume_count++;
        }
        EXPECT_EQ(consume_count, TotalItems);
    }

} // namespace