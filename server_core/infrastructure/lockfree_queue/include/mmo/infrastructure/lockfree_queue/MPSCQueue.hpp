// /home/xj/projects/mmo_server/server_core/infrastructure/lockfree_queue/include/mmo/infrastructure/lockfree_queue/MPSCQueue.hpp
#pragma once

#include <array>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <cassert>
#include <utility>

#include <mmo/infrastructure/lockfree_queue/CacheAligned.hpp>
#include <mmo/infrastructure/lockfree_queue/QueueSlot.hpp>
#include <mmo/infrastructure/lockfree_queue/QueueTypes.hpp>

namespace mmo::infrastructure::lockfree_queue
{
    template <typename T, std::size_t Capacity>
    class MPSCQueue
    {
        // ==========================================
        // 四、第一部分：静态断言（编译期防御性设计）
        // ==========================================
        static_assert(Capacity >= 2, "Queue capacity must be >= 2.");
        static_assert((Capacity & (Capacity - 1)) == 0, "Queue capacity must be power-of-two.");

    public:
        // ==========================================
        // 五、Constructor & Destructor（所有权初始化）
        // ==========================================
        MPSCQueue()
            : m_enqueuePos(0),
              m_dequeuePos(0)
        {
            // 六、为什么 sequence = i？这是 Slot 的生命周期通行证
            for (std::size_t i = 0; i < Capacity; ++i)
            {
                m_slots[i].sequence.store(static_cast<QueueSequence>(i), std::memory_order_relaxed);
            }
        }

        ~MPSCQueue() = default;

        // 显式禁止拷贝与移动，防止无锁容器地址漂移导致灾难
        MPSCQueue(const MPSCQueue &) = delete;
        MPSCQueue &operator=(const MPSCQueue &) = delete;
        MPSCQueue(MPSCQueue &&) = delete;
        MPSCQueue &operator=(MPSCQueue &&) = delete;

    public:
        // ==========================================
        // 七、Enqueue（多生产者无锁预留算法 - Copy 语义）
        // ==========================================
        bool Enqueue(const T &item)
        {
            QueueSequence pos = m_enqueuePos.load(std::memory_order_relaxed);

            for (;;)
            {
                // 使用位运算代替昂贵的模运算 (%)
                QueueSlot<T> &slot = m_slots[pos & (Capacity - 1)];
                QueueSequence seq = slot.sequence.load(std::memory_order_acquire);

                // 💡 算法关键点：利用无符号数减法回绕特征，完美处理临界区数据溢出
                intptr_t diff = static_cast<intptr_t>(seq - pos);

                if (diff == 0)
                {
                    // 八、Producer Claim: 抢占当前 Slot 索引
                    if (m_enqueuePos.compare_exchange_weak(pos, pos + 1, std::memory_order_relaxed))
                    {
                        slot.storage = item;

                        // 九、Release Publish: 递增当前 Slot 序列号，激活通知 Consumer
                        slot.sequence.store(pos + 1, std::memory_order_release);
                        return true;
                    }
                }
                else if (diff < 0)
                {
                    // 缓冲槽还没被消费，说明队列已满（Bounded 边界撞墙）
                    return false;
                }
                else
                {
                    // diff > 0: 说明其他 Producer 捷足先登了，重新加载位置重试
                    pos = m_enqueuePos.load(std::memory_order_relaxed);
                }
            }
        }

        // ==========================================
        // Enqueue（多生产者无锁预留算法 - Move 语义支持）
        // ==========================================
        bool Enqueue(T &&item)
        {
            QueueSequence pos = m_enqueuePos.load(std::memory_order_relaxed);

            for (;;)
            {
                QueueSlot<T> &slot = m_slots[pos & (Capacity - 1)];
                QueueSequence seq = slot.sequence.load(std::memory_order_acquire);
                intptr_t diff = static_cast<intptr_t>(seq - pos);

                if (diff == 0)
                {
                    if (m_enqueuePos.compare_exchange_weak(pos, pos + 1, std::memory_order_relaxed))
                    {
                        slot.storage = std::move(item); // 移动语义减少游戏 Packet 的拷贝开销
                        slot.sequence.store(pos + 1, std::memory_order_release);
                        return true;
                    }
                }
                else if (diff < 0)
                {
                    return false;
                }
                else
                {
                    pos = m_enqueuePos.load(std::memory_order_relaxed);
                }
            }
        }

        // ==========================================
        // 十、Dequeue（单消费者绝对优化版）
        // ==========================================
        bool Dequeue(T &out)
        {
            QueueSlot<T> &slot = m_slots[m_dequeuePos & (Capacity - 1)];
            QueueSequence seq = slot.sequence.load(std::memory_order_acquire);
            intptr_t diff = static_cast<intptr_t>(seq - (m_dequeuePos + 1));

            if (diff == 0)
            {
                out = std::move(slot.storage);

                // 十一、Slot Recycle: 将控制权归还给未来的 Producer
                // 标记该位置可以被下一次循环的生产者使用
                slot.sequence.store(m_dequeuePos + Capacity, std::memory_order_release);

                // 💡 MPSC 终极优化：因为只有一个消费者，m_dequeuePos 不需要是原子变量，直接递增
                ++m_dequeuePos;
                return true;
            }

            return false;
        }

    public:
        // ==========================================
        // 十二、辅助度量接口（估算值）
        // ==========================================
        [[nodiscard]] bool Empty() const noexcept
        {
            return ApproxSize() == 0;
        }

        [[nodiscard]] std::size_t CapacityValue() const noexcept
        {
            return Capacity;
        }

        [[nodiscard]] std::size_t ApproxSize() const noexcept
        {
            QueueSequence enqueue = m_enqueuePos.load(std::memory_order_relaxed);
            // 依旧利用单消费者优势，无锁状态下安全作差
            return static_cast<std::size_t>(enqueue - m_dequeuePos);
        }

    private:
        using SlotArray = std::array<QueueSlot<T>, Capacity>;

    private:
        // 独占一个 CacheLine，防止外界数组对首个 Slot 造成干扰
        alignas(kCacheLineSize) SlotArray m_slots;

        // 生产者高频竞争的原子边界，必须独占 CacheLine
        alignas(kCacheLineSize) std::atomic<QueueSequence> m_enqueuePos;
        char m_pad0[kCacheLineSize]; // 显式防护伪共享

        // 消费者独占的非原子私有边界，杜绝被多生产者的原子 CAS 污染缓存行
        alignas(kCacheLineSize) QueueSequence m_dequeuePos;
        char m_pad1[kCacheLineSize];
    };
}