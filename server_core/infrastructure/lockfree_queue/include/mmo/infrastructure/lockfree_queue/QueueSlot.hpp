// /home/xj/projects/mmo_server/server_core/infrastructure/lockfree_queue/include/mmo/infrastructure/lockfree_queue/QueueSlot.hpp
#pragma once

#include <atomic>
#include <optional>

#include <mmo/infrastructure/lockfree_queue/QueueTypes.hpp>

namespace mmo::infrastructure::lockfree_queue
{

    template <typename T>
    struct alignas(kCacheLineSize) QueueSlot
    {
        std::atomic<QueueSequence> sequence{0};

        T storage{};
    };

    static_assert(
        std::is_standard_layout_v<QueueSlot<std::uint32_t>>,
        "QueueSlot must satisfy standard layout to guarantee binary predictability.");

    static_assert(
        std::is_trivially_destructible_v<QueueSlot<std::uint32_t>>,
        "QueueSlot must be trivially destructible to avoid unexpected cleanup overhead in hot paths.");

    static_assert(
        alignof(QueueSlot<std::uint32_t>) == kCacheLineSize,
        "QueueSlot alignment must strictly match CPU cache line size (64 bytes).");
}