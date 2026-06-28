// /home/xj/projects/mmo_server/server_core/runtime/TickScheduler/include/mmo/runtime/TickScheduler/TickSchedulerTypes.hpp
#pragma once

#include <cstdint>

namespace mmo::runtime
{

    enum class TickSchedulerState : std::uint8_t
    {
        Created = 0,
        Initialized,
        Running,
        Stopping,
        Stopped
    };

} // namespace mmo::runtime