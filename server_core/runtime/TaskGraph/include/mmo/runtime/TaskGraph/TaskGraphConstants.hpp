// /home/xj/projects/mmo_server/server_core/runtime/TaskGraph/include/mmo/runtime/TaskGraph/TaskGraphConstants.hpp
#pragma once

#include "mmo/runtime/TaskGraph/TaskGraphTypes.hpp"

namespace mmo::runtime
{

    inline constexpr TaskNodeCount TaskGraphMaxNodesV1 = 256;

    inline constexpr TaskDependencyCount TaskGraphMaxDependenciesV1 = 1024;

}