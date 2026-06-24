#include <gtest/gtest.h>
#include <array>
#include <cstring>
#include <memory>
#include <thread>
#include <atomic>

#include <mmo/infrastructure/lockfree_queue/MPSCQueue.hpp>

using namespace mmo::infrastructure::lockfree_queue;

namespace
{
    struct Message
    {
        uint64_t id;
        std::array<char, 4096> payload; // 4KB 大对象
    };
}

TEST(MPSCQueueRuntimeTest, LargePayloadTransfer)
{
    // 采用 64 容量的窄队列，强制引发槽位的高频高密度复用与析构对撞
    MPSCQueue<std::unique_ptr<Message>, 64> queue;
    constexpr uint64_t kMessageCount = 50000;
    std::atomic<bool> consumer_done{false};

    // 1. 异步消费者线程：在另一个核心上高频弹回并触发真正的 delete
    std::thread consumer([&]
                         {
        uint64_t expected_id = 0;
        while (expected_id < kMessageCount)
        {
            std::unique_ptr<Message> out;
            if (queue.Dequeue(out))
            {
                ASSERT_NE(out, nullptr);
                ASSERT_EQ(out->id, expected_id);

                // 验证动态堆内存的高保真度，防止移动语义导致内部指针断裂
                for (char c : out->payload)
                {
                    ASSERT_EQ(static_cast<unsigned char>(c), static_cast<unsigned char>(expected_id % 255));
                }
                expected_id++;
                // 💡 unique_ptr 离开作用域，在消费者线程上下文中释放内存。
                // 这将与生产者的 slot.storage = std::move(item) 在 TSan 下进行真正的Happens-Before对撞审计！
            }
            else
            {
#if defined(__x86_64__) || defined(_M_X64)
                __builtin_ia32_pause();
#else
                std::this_thread::yield();
#endif
            }
        }
        consumer_done.store(true); });

    // 2. 主线程作为生产者：全速负责投递
    for (uint64_t i = 0; i < kMessageCount; ++i)
    {
        auto msg = std::make_unique<Message>();
        msg->id = i;
        std::memset(msg->payload.data(), static_cast<int>(i % 255), msg->payload.size());

        // 如果队列慢了，采用智能自旋退避，等待消费者腾出槽位
        while (!queue.Enqueue(std::move(msg)))
        {
#if defined(__x86_64__) || defined(_M_X64)
            __builtin_ia32_pause();
#else
            std::this_thread::yield();
#endif
        }
    }

    if (consumer.joinable())
    {
        consumer.join();
    }

    ASSERT_TRUE(consumer_done.load());
}