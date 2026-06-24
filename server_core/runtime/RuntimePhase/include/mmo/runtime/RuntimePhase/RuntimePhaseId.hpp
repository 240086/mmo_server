// /home/xj/projects/mmo_server/server_core/runtime/RuntimePhase/include/mmo/runtime/RuntimePhase/RuntimePhaseId.hpp
#pragma once

#include <cstdint>

namespace mmo::runtime
{

    enum class RuntimePhaseId : std::uint8_t
    {
        Input = 0,

        Simulation,

        PostSimulation,

        Output
    };

} // namespace mmo::runtime