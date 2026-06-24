// /home/xj/projects/mmo_server/server_core/runtime/RuntimeContext/include/mmo/runtime/RuntimeContext/RuntimeMode.hpp
#pragma once

#include <cstdint>

namespace mmo::runtime
{

    enum class RuntimeMode : std::uint8_t
    {
        Initializing = 0,

        Running,

        Stopping,

        Stopped
    };

} // namespace mmo::runtime