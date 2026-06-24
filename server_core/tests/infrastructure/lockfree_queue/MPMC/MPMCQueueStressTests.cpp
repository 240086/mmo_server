#include <gtest/gtest.h>
#include <atomic>
#include <thread>
#include <vector>
#include <memory>
#include <mmo/infrastructure/lockfree_queue/MPMCQueue.hpp>

using namespace mmo::infrastructure::lockfree_queue;

namespace
{
    TEST(MPMCQueueStressTests, OneMillionOperations)
    {
        constexpr int Count = 1000000;

        // 🎯 优化一：提升到堆上分发，规避栈开销
        auto queue = std::make_unique<MPMCQueue<int, 4096>>();

        std::thread producer(
            [&]()
            {
                for (int i = 0; i < Count; ++i)
                {
                    while (!queue->Enqueue(i))
                    {
                        // 🎯 优化二：盲自旋解毒剂。
                        // 在裸跑时它几乎不影响性能，但在 Valgrind 模拟器或单核 CI 虚拟机中，
                        // 它会强制触发内核/Valgrind 的调度器切出当前线程，彻底打破活锁。
                        std::this_thread::yield();
                    }
                }
            });

        std::thread consumer(
            [&]()
            {
                int value;
                int received = 0;

                while (received < Count)
                {
                    if (queue->Dequeue(value))
                    {
                        ++received;
                    }
                    else
                    {
                        // 🎯 优化三：队列为空时，消费者也应同样温和地让出 CPU
                        std::this_thread::yield();
                    }
                }
            });

        producer.join();
        consumer.join();

        SUCCEED();
    }
} // namespace