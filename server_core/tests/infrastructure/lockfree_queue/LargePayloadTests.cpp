// /home/xj/projects/mmo_server/server_core/tests/infrastructure/lockfree_queue/LargePayloadTests.cpp
#include <gtest/gtest.h>
#include <array>
#include <cstring>
#include <memory>

#include <mmo/infrastructure/lockfree_queue/MPSCQueue.hpp>

using namespace mmo::infrastructure::lockfree_queue;

namespace
{
    // 模拟真实的工业级 MMO 巨型网络包或战斗快照
    struct Message
    {
        uint64_t id;
        std::array<char, 4096> payload; // 4KB 大对象
    };
}

TEST(MPSCQueueRuntimeTest, LargePayloadTransfer)
{
    // 🎯 架构师重构：存储 unique_ptr，彻底杜绝 4KB 巨型内存的隐式显式拷贝！
    // 此时无锁队列在微观交换时，只传输 8 字节的堆指针，性能登顶！
    MPSCQueue<std::unique_ptr<Message>, 1024> queue;

    constexpr uint64_t kMessageCount = 1000;

    // 1. 生产者投递阶段：使用纯正移动语义投递指针
    for (uint64_t i = 0; i < kMessageCount; ++i)
    {
        // 在堆上安全分配大对象，防撑爆测试栈
        auto msg = std::make_unique<Message>();
        msg->id = i;

        std::memset(
            msg->payload.data(),
            static_cast<int>(i % 255),
            msg->payload.size());

        // 真正的无损移动：仅仅转移堆所有权（交换 8 字节指针值）
        ASSERT_TRUE(queue.Enqueue(std::move(msg)));
    }

    // 2. 消费者消费阶段
    for (uint64_t i = 0; i < kMessageCount; ++i)
    {
        std::unique_ptr<Message> out;

        // 弹出指针
        ASSERT_TRUE(queue.Dequeue(out));

        // 确保指针合法，没有发生中途断裂
        ASSERT_NE(out, nullptr);
        ASSERT_EQ(out->id, i);

        // 验证大对象的内存高保真度
        for (char c : out->payload)
        {
            ASSERT_EQ(
                static_cast<unsigned char>(c),
                static_cast<unsigned char>(i % 255));
        }

        // 离开作用域，unique_ptr 自动原位释放物理堆内存
    }
}