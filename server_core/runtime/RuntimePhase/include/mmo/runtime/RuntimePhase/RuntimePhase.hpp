// /home/xj/projects/mmo_server/server_core/runtime/RuntimePhase/include/mmo/runtime/RuntimePhase/RuntimePhase.hpp
#pragma once

#include "mmo/runtime/RuntimePhase/RuntimePhaseId.hpp"

namespace mmo::runtime
{

    struct RuntimePhase final
    {
        RuntimePhaseId id{RuntimePhaseId::Input};
    };

} // namespace mmo::runtime