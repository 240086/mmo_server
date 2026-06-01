// /home/xj/projects/mmo_server/server_core/infrastructure/lockfree_queue/include/mmo/infrastructure/lockfree_queue/QueueSlot.hpp
#pragma once

#include <atomic>
#include <optional>

#include <mmo/infrastructure/lockfree_queue/QueueTypes.hpp>

namespace mmo::infrastructure::lockfree_queue
{

    template <typename T>
    struct QueueSlot
    {
        std::atomic<QueueSequence> sequence;

        T storage;
    };

}