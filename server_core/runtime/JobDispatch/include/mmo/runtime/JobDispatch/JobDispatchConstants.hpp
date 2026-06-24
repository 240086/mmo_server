// /home/xj/projects/mmo_server/server_core/runtime/JobDispatch/include/mmo/runtime/JobDispatch/JobDispatchConstants.hpp
#pragma once

#include <cstddef>
#include <mmo/runtime/TaskGraph/TaskGraphConstants.hpp>

namespace mmo::runtime
{

    /**
     * @brief JobDispatch 模块编译期核心常量
     */
    // constexpr std::size_t TaskGraphMaxNodesV1 = 256;
    constexpr std::size_t kCacheLineSize = 64;

} // namespace mmo::runtime