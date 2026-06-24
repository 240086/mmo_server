// /home/xj/projects/mmo_server/server_core/tests/infrastructure/lockfree_queue/MPMC/MPMCQueueWrapAroundTests.cpp
#include <gtest/gtest.h>
#include <memory>
#include <mmo/infrastructure/lockfree_queue/MPMCQueue.hpp>

namespace mmo::infrastructure::lockfree_queue
{
    struct MPMCQueueTestInspector
    {
        // 🎯 升级版特权通道：不仅修改游标，还要对齐整个环形缓冲区的内部状态机序列号
        template <typename T, std::size_t Cap>
        static void PrepareWrapAroundState(MPMCQueue<T, Cap> &queue, uint64_t val) noexcept
        {
            // 1. 同步推进头尾游标
            queue.m_head.store(val, std::memory_order_relaxed);
            queue.m_tail.store(val, std::memory_order_relaxed);

            // 2. 🎯 核心修复：根据当前回绕基准，重新对齐所有物理 Slot 的控制序列号
            // 假设你的 MPMCQueue 内部存储槽位的数组叫 m_slots
            // 并且每个 Slot 内部用来控制同步的变量叫 m_sequence
            // (请根据你 MPMCQueue.hpp 里的实际变量名微调，例如 queue.m_buffer)
            for (std::size_t i = 0; i < Cap; ++i)
            {
                // 槽位的绝对序列号 = 基础高位值 + 槽位索引
                // 例如基准是 0xFFFFFFFFFFFFFFF0，那么第 i 个槽位的初始预期序列号就是 val + i
                queue.m_slots[i].sequence.store(val + i, std::memory_order_relaxed);
            }
        }

        template <typename T, std::size_t Cap>
        static uint64_t GetLength(const MPMCQueue<T, Cap> &queue) noexcept
        {
            return queue.m_tail.load(std::memory_order_relaxed) - queue.m_head.load(std::memory_order_relaxed);
        }
    };
} // namespace mmo::infrastructure::lockfree_queue

namespace
{
    using namespace mmo::infrastructure::lockfree_queue;

    TEST(MPMCQueueWrapAroundTests, RingBufferIntegerOverflow)
    {
        // 使用 heap 分配防止未来扩容爆栈
        auto queue = std::make_unique<MPMCQueue<int, 16>>();

        // 🎯 跨越绝对零度临界边缘：0xFFFFFFFFFFFFFFF0
        uint64_t critical_edge = 0xFFFFFFFFFFFFFFF0ULL;

        // 🎯 激活状态机全量对齐
        MPMCQueueTestInspector::PrepareWrapAroundState(*queue, critical_edge);

        // 跨越绝对零度：迭代 128 次，保证游标能够跨越 0xFFFFFFFFFFFFFFFF 顺利安全翻转回 0x0
        constexpr int Iterations = 128;

        for (int i = 0; i < Iterations; ++i)
        {
            ASSERT_TRUE(queue->Enqueue(i))
                << "💥 Queue blocked during integer wrap-around at step: " << i;

            int value = -1;
            ASSERT_TRUE(queue->Dequeue(value))
                << "💥 Queue failed to dequeue during integer wrap-around at step: " << i;

            ASSERT_EQ(value, i);
        }

        // 最终状态核验
        EXPECT_EQ(MPMCQueueTestInspector::GetLength(*queue), 0u);
    }

} // namespace