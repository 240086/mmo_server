// /home/xj/projects/mmo_server/server_core/runtime/JobDispatch/include/mmo/runtime/JobDispatch/JobDispatchTypes.hpp
#pragma once

#include <cstdint>
#include "mmo/runtime/TaskGraph/TaskGraphTypes.hpp"

namespace mmo::runtime
{

    // 显式拉取上游拓扑层的类型定义，保持 ABI 强一致
    using TaskId = mmo::runtime::TaskId;

    using DependencyCount = std::uint32_t;

    using WorkerIndex = std::uint16_t;

}