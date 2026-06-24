// /home/xj/projects/mmo_server/server_core/runtime/JobDispatch/include/mmo/runtime/JobDispatch/WorkerThread.hpp
#pragma once

#include <thread>
#include "mmo/runtime/JobDispatch/JobDispatchTypes.hpp"

namespace mmo::runtime
{

    /**
     * @brief 原生物理线程强力隔离包装层
     */
    struct WorkerThread final
    {
        std::thread thread;

        WorkerIndex workerIndex;

        std::uint64_t executedTasks;

        std::uint64_t stealCount;
    };

} // namespace mmo::runtime