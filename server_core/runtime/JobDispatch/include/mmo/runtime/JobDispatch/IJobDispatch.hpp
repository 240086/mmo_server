// /home/xj/projects/mmo_server/server_core/runtime/JobDispatch/include/mmo/runtime/JobDispatch/IJobDispatch.hpp
#pragma once

#include "mmo/runtime/RuntimePhase/RuntimePhaseId.hpp"

namespace mmo::runtime
{
    class TaskGraph;
    class RuntimeContext;
    class TaskRegistry;
    /**
     * @brief 核心执行引擎的高层抽象控制面契约
     */
    class IJobDispatch
    {
    public:
        virtual ~IJobDispatch() = default;

    public:
        /**
         * @brief 初始化调度器环境并冷启动物理线程资源
         */
        virtual bool Initialize(
            const TaskGraph &graph,
            RuntimeContext &context,
            const TaskRegistry &registry) = 0;

        /**
         * @brief 驱动并驱动特定 Phase 的拓扑任务步进
         * @param phaseId 当前执行的目标流水线切片阶段 (Gameplay, Physics, Net 等)
         */
        virtual bool ExecutePhase(RuntimePhaseId phaseId) = 0;

        /**
         * @brief 安全 Join 物理线程并清空上下文状态
         */
        virtual void Shutdown() = 0;
    };

} // namespace mmo::runtime