// /home/xj/projects/mmo_server/server_core/runtime/TickScheduler/include/mmo/runtime/TickScheduler/TickSchedulerConfig.hpp
#pragma once

#include <chrono>

namespace mmo::runtime
{

    struct TickSchedulerConfig
    {
        std::chrono::milliseconds tickInterval{
            50};
    };

} // namespace mmo::runtime