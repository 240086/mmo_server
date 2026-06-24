// /home/xj/projects/mmo_server/server_core/runtime/JobDispatch/include/mmo/runtime/JobDispatch/ITaskCompletionSink.hpp
#pragma once

#include "mmo/runtime/JobDispatch/JobDispatchTypes.hpp"

namespace mmo::runtime
{

    /**
     * @brief 任务完成事件的非对称接收汇
     * @note 彻底阻断 JobDispatch 与 WorkerPool 之间的循环包含纽带
     */
    class ITaskCompletionSink
    {
    public:
        virtual ~ITaskCompletionSink() = default;

    public:
        /**
         * @brief 物理线程执行完毕后的低损耗回调通知面
         * @param taskId 已安全跑完的目标任务 ID
         */
        virtual void OnTaskCompleted(TaskId taskId) noexcept = 0;
    };

} // namespace mmo::runtime