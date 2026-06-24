// /home/xj/projects/mmo_server/server_core/runtime/JobDispatch/include/mmo/runtime/JobDispatch/WorkerContext.hpp
#pragma once

#include "mmo/runtime/JobDispatch/JobDispatchTypes.hpp"

namespace mmo::runtime
{

    /**
     * @brief 工作线程专有上下文
     * @note 未来挂载：NUMA 节点亲和性标识、CPU Affinity 掩码、每线程 Profiling 采样器及 Tracing 轨迹句柄
     */
    struct WorkerContext final
    {
        WorkerIndex workerIndex{0};
    };

} // namespace mmo::runtime