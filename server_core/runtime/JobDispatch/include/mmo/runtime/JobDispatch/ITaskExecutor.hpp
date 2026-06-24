// /home/xj/projects/mmo_server/server_core/runtime/JobDispatch/include/mmo/runtime/JobDispatch/ITaskExecutor.hpp
#pragma once

#include "mmo/runtime/JobDispatch/JobDispatchTypes.hpp"

namespace mmo::runtime
{

    /**
     * @brief 任务执行器的终极解耦面
     * @note 彻底阻断 WorkerPool 对 TaskRegistry 和 RuntimeContext 的领域感知
     */
    class ITaskExecutor
    {
    public:
        virtual ~ITaskExecutor() = default;

    public:
        /**
         * @brief 执行指定的拓扑任务
         * @param taskId 需要被执行的物理任务 ID
         */
        virtual void ExecuteTask(TaskId taskId) noexcept = 0;
    };

} // namespace mmo::runtime