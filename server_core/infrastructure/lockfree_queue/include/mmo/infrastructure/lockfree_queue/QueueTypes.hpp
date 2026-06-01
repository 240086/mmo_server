// /home/xj/projects/mmo_server/server_core/infrastructure/lockfree_queue/include/mmo/infrastructure/lockfree_queue/QueueTypes.hpp
#pragma once

#include <cstddef>
#include <cstdint>

namespace mmo::infrastructure::lockfree_queue
{

    using QueueSequence = std::uint64_t;
    using QueueIndex = std::size_t;

    constexpr std::size_t kCacheLineSize = 64;

}