// /home/xj/projects/mmo_server/server_core/infrastructure/lockfree_queue/include/mmo/infrastructure/lockfree_queue/MPMCQueue.hpp
#pragma once

#include <array>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <type_traits>
#include <utility>

#include <mmo/infrastructure/lockfree_queue/CacheAligned.hpp>
#include <mmo/infrastructure/lockfree_queue/QueueSlot.hpp>

// TSan 动态分析工具前置声明，防止无锁环境下的影子内存误报
#if defined(__SANITIZE_THREAD__)
extern "C" void __tsan_acquire(void *addr);
extern "C" void __tsan_release(void *addr);
#endif

namespace mmo::infrastructure::lockfree_queue
{
    struct MPMCQueueTestInspector;
    
    template <typename T, std::size_t Capacity>
    class MPMCQueue final
    {
        friend struct MPMCQueueTestInspector;

        static_assert(Capacity >= 2, "Queue capacity must be >= 2.");
        static_assert((Capacity & (Capacity - 1)) == 0, "Queue capacity must be a power-of-two.");

    public:
        /**
         * @brief 构造函数：初始化环形缓冲区
         * @note 依据 Vyukov 算法，每个槽位的初始序列号设为其自身的物理索引位置
         */
        MPMCQueue()
            : m_head(0),
              m_tail(0)
        {
            for (std::size_t i = 0; i < Capacity; ++i)
            {
                m_slots[i].sequence.store(static_cast<QueueSequence>(i), std::memory_order_relaxed);
            }
        }

        ~MPMCQueue() = default;

        // 严禁复制与移动，杜绝多线程环境下的句柄漂移与所有权无序传递
        MPMCQueue(const MPMCQueue &) = delete;
        MPMCQueue &operator=(const MPMCQueue &) = delete;
        MPMCQueue(MPMCQueue &&) = delete;
        MPMCQueue &operator=(MPMCQueue &&) = delete;

    public:
        /**
         * @brief 右值移动压入队列 (Hot Path)
         */
        bool Enqueue(T &&item)
        {
            QueueSequence pos = m_tail.load(std::memory_order_relaxed);

            for (;;)
            {
                // 利用 Power-of-Two 特性，位与操作完美替代高耗时的取模 (%) 运算
                QueueSlot<T> &slot = m_slots[pos & (Capacity - 1)];
                QueueSequence seq = slot.sequence.load(std::memory_order_acquire);
                std::intptr_t diff = static_cast<std::intptr_t>(seq - pos);

                if (diff == 0)
                {
                    // 抢占 tail 计数器。若失败，证明其他生产者先一步夺走了该槽位，CAS 失败并重试
                    if (m_tail.compare_exchange_weak(pos, pos + 1, std::memory_order_relaxed, std::memory_order_relaxed))
                    {
                        slot.storage = std::move(item);

#if defined(__SANITIZE_THREAD__)
                        __tsan_release(&slot);
#endif
                        // 关键同步点：Release 语义发布更新，通知消费者此槽位已就绪
                        slot.sequence.store(pos + 1, std::memory_order_release);
                        return true;
                    }
                }
                else if (diff < 0)
                {
                    // 槽位的序列号落后于期待值，说明底层环形缓冲区已满且还未被消费者消费
                    return false;
                }
                else
                {
                    // 读取到了陈旧的 tail 计数，重新加载以准备进入下一轮自旋 CAS
                    pos = m_tail.load(std::memory_order_relaxed);
                }
            }
        }

        /**
         * @brief 左值复制压入队列
         */
        bool Enqueue(const T &item)
        {
            T copy = item;
            return Enqueue(std::move(copy));
        }

        /**
         * @brief 弹出队列 (Hot Path)
         */
        bool Dequeue(T &out)
        {
            QueueSequence pos = m_head.load(std::memory_order_relaxed);

            for (;;)
            {
                QueueSlot<T> &slot = m_slots[pos & (Capacity - 1)];
                QueueSequence seq = slot.sequence.load(std::memory_order_acquire);
                std::intptr_t diff = static_cast<std::intptr_t>(seq - (pos + 1));

                if (diff == 0)
                {
                    // 抢占 head 计数器。若失败，证明其他消费者先一步劫走了该槽位，CAS 失败并重试
                    if (m_head.compare_exchange_weak(pos, pos + 1, std::memory_order_relaxed, std::memory_order_relaxed))
                    {
#if defined(__SANITIZE_THREAD__)
                        __tsan_acquire(&slot);
#endif
                        out = std::move(slot.storage);

                        // 关键同步点：Release 语义发布更新，将序列号递增为下一次绕回时的期待值，宣告该槽位已空闲
                        slot.sequence.store(pos + Capacity, std::memory_order_release);
                        return true;
                    }
                }
                else if (diff < 0)
                {
                    // 槽位序列号不足，说明生产者还没有将数据填入此位置，队列当前为空
                    return false;
                }
                else
                {
                    // 读取到了陈旧的 head 计数，重新加载并进入下一轮自旋 CAS
                    pos = m_head.load(std::memory_order_relaxed);
                }
            }
        }

    public:
        /**
         * @brief 近似判定队列是否为空
         */
        [[nodiscard]] bool Empty() const noexcept
        {
            return ApproxSize() == 0;
        }

        /**
         * @brief 获取当前队列近似的大小 (Best Effort)
         * @note 由于 MPMC 存在极端并发现象，该值仅供 Metrics 统计或辅助判定，绝不可作为严格的同步依据
         */
        [[nodiscard]] std::size_t ApproxSize() const noexcept
        {
            QueueSequence tail = m_tail.load(std::memory_order_relaxed);
            QueueSequence head = m_head.load(std::memory_order_acquire);

            return static_cast<std::size_t>(tail >= head ? (tail - head) : 0);
        }

        [[nodiscard]] constexpr std::size_t CapacityValue() const noexcept
        {
            return Capacity;
        }

    private:
        using SlotArray = std::array<QueueSlot<T>, Capacity>;

    private:
        // ============================================================================
        // 物理内存布局规划：严格执行缓存行隔离
        // ============================================================================

        // 整个槽位数组常驻独立内存区域
        alignas(kCacheLineSize) SlotArray m_slots;

        // 生产者和消费者核心索引彻底拉开 64 字节距离，杜绝多核 CPU 核心之间的锁总线反弹
        alignas(kCacheLineSize) std::atomic<QueueSequence> m_head;
        alignas(kCacheLineSize) std::atomic<QueueSequence> m_tail;

        // 尾部防溢出填充，防止该类实例后面紧跟的业务变量入侵 m_tail 的缓存行
        char m_pad_end[kCacheLineSize];
    };

} // namespace mmo::infrastructure::lockfree_queue