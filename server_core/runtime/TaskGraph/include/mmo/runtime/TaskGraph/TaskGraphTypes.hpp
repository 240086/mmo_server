// /home/xj/projects/mmo_server/server_core/runtime/TaskGraph/include/mmo/runtime/TaskGraph/TaskGraphTypes.hpp
#pragma once

#include <cstdint>

namespace mmo::runtime
{

    using TaskId = std::uint16_t;

    inline constexpr TaskId InvalidTaskId = 0xFFFF;

    using TaskNodeCount = std::uint16_t;

    using TaskDependencyCount = std::uint16_t;

}