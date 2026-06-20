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

namespace mmo::infrastructure::lockfree_queue
{

    template <typename T, std::size_t Capacity>
    class MPSCQueue
    {
        static_assert(Capacity >= 2, "Queue capacity must be >= 2.");
        static_assert((Capacity & (Capacity - 1)) == 0, "Queue capacity must be power-of-two.");

    public:
        MPSCQueue()
            : m_enqueuePos(0),
              m_dequeuePos(0)
        {
            for (std::size_t i = 0; i < Capacity; ++i)
            {
                m_slots[i].sequence.store(static_cast<QueueSequence>(i), std::memory_order_relaxed);
            }
        }

        ~MPSCQueue() = default;

        MPSCQueue(const MPSCQueue &) = delete;
        MPSCQueue &operator=(const MPSCQueue &) = delete;
        MPSCQueue(MPSCQueue &&) = delete;
        MPSCQueue &operator=(MPSCQueue &&) = delete;

    public:
        // ==========================================
        // Enqueue (Move 语义支持)
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
                    if (m_enqueuePos.compare_exchange_weak(pos, pos + 1, std::memory_order_acq_rel, std::memory_order_relaxed))
                    {
                        slot.storage = std::move(item);

                        // 🎯 完美绕过限制：在 TSan 环境下使用官方专用 API 同步影子内存，非 TSan 环境下直接交由原生的 Acquire/Release 隐式担保
#if defined(__SANITIZE_THREAD__)
                        __tsan_release(&slot);
#endif

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

        bool Enqueue(const T &item)
        {
            T copy = item;
            return Enqueue(std::move(copy));
        }

        // ==========================================
        // Dequeue (单消费者重构优化版)
        // ==========================================
        bool Dequeue(T &out)
        {
            // 🌟 修复 Problem 2：从原子变量中加载当前消费位置
            QueueSequence deqPos = m_dequeuePos.load(std::memory_order_relaxed);

            QueueSlot<T> &slot = m_slots[deqPos & (Capacity - 1)];
            QueueSequence seq = slot.sequence.load(std::memory_order_acquire);
            intptr_t diff = static_cast<intptr_t>(seq - (deqPos + 1));

            if (diff == 0)
            {
                // 🎯 显式告知 TSan：这里安全获取了当前 slot 地址的所有权（包括其内部指针指向的堆内存）
#if defined(__SANITIZE_THREAD__)
                __tsan_acquire(&slot);
#endif
                out = std::move(slot.storage);

                slot.sequence.store(deqPos + Capacity, std::memory_order_release);

                // 🌟 修复 Problem 2：使用 release 语义发布更新，消除 ApproxSize 的数据竞争 UB
                m_dequeuePos.store(deqPos + 1, std::memory_order_release);
                return true;
            }

            return false;
        }

    public:
        [[nodiscard]] bool Empty() const noexcept
        {
            return ApproxSize() == 0;
        }

        [[nodiscard]] std::size_t CapacityValue() const noexcept
        {
            return Capacity;
        }

        // ==========================================
        // 🌟 修复 Problem 2：彻底安全的度量接口
        // ==========================================
        [[nodiscard]] std::size_t ApproxSize() const noexcept
        {
            QueueSequence enqueue = m_enqueuePos.load(std::memory_order_relaxed);
            QueueSequence dequeue = m_dequeuePos.load(std::memory_order_acquire);

            // 无符号回绕安全作差
            return static_cast<std::size_t>(enqueue - dequeue);
        }

    private:
        using SlotArray = std::array<QueueSlot<T>, Capacity>;

    private:
        // 各自独占 CacheLine，严防伪共享
        alignas(kCacheLineSize) SlotArray m_slots;
        alignas(kCacheLineSize) std::atomic<QueueSequence> m_enqueuePos;

        // 🌟 修复 Problem 2：升级为原子变量并独占缓存行
        alignas(kCacheLineSize) std::atomic<QueueSequence> m_dequeuePos;
        char m_pad_end[kCacheLineSize];
    };
}