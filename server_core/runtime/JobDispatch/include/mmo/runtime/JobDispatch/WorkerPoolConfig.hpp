// /home/xj/projects/mmo_server/server_core/runtime/JobDispatch/include/mmo/runtime/JobDispatch/WorkerPoolConfig.hpp
#pragma once

#include "mmo/infrastructure/lockfree_queue/MPMCQueue.hpp"
#include "mmo/runtime/JobDispatch/JobDispatchConstants.hpp"
#include "mmo/runtime/JobDispatch/JobDispatchTypes.hpp"

namespace mmo::runtime
{

    class RuntimeContext;
    class TaskRegistry;
    class ITaskCompletionSink;

    /**
     * @brief Worker 物理基础设施冷启动静态配置面
     * @note 采用纯原生指针传递，杜绝智能指针带来的引用计数颠簸与 Cache Miss
     */
    struct WorkerPoolConfig final
    {
        RuntimeContext *runtimeContext{nullptr};
        const TaskRegistry *registry{nullptr};
        mmo::infrastructure::lockfree_queue::MPMCQueue<TaskId, TaskGraphMaxNodesV1> *readyQueue{nullptr};
        ITaskCompletionSink *completionSink{nullptr};
    };

} // namespace mmo::runtime