// /home/xj/projects/mmo_server/server_core/infrastructure/lockfree_queue/include/mmo/infrastructure/lockfree_queue/CacheAligned.hpp
#pragma once

#include <mmo/infrastructure/lockfree_queue/QueueTypes.hpp>

namespace mmo::infrastructure::lockfree_queue
{

    template <typename T>
    struct alignas(kCacheLineSize) CacheAligned
    {
        T value;
    };

}