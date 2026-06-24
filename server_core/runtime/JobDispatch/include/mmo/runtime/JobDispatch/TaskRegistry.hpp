// /home/xj/projects/mmo_server/server_core/runtime/JobDispatch/include/mmo/runtime/JobDispatch/TaskRegistry.hpp
#pragma once

#include <array>
#include "mmo/runtime/JobDispatch/JobDispatchConstants.hpp"
#include "mmo/runtime/JobDispatch/JobDispatchTypes.hpp"
#include "mmo/runtime/JobDispatch/TaskFunction.hpp"

namespace mmo::runtime
{
    // 前置声明解耦，拒绝对齐非必要领域依赖
    class TaskGraph;
    /**
     * @brief 编译期无锁、静态寻址的任务注册表
     */
    class TaskRegistry final
    {
    public:
        TaskRegistry() noexcept = default;
        ~TaskRegistry() = default;

        // 禁用移动与复制契约
        TaskRegistry(const TaskRegistry &) = delete;
        TaskRegistry &operator=(const TaskRegistry &) = delete;
        TaskRegistry(TaskRegistry &&) = delete;
        TaskRegistry &operator=(TaskRegistry &&) = delete;

    public:
        /**
         * @brief 静态绑定任务 ID 与具体执行指针
         */
        inline bool Register(TaskId id, TaskFunction function) noexcept
        {
            if (id >= TaskGraphMaxNodesV1) [[unlikely]]
            {
                return false;
            }
            m_registry[id] = function;
            return true;
        }

        /**
         * @brief O(1) 物理槽位快速解算
         */
        [[nodiscard]] inline TaskFunction Resolve(TaskId id) const noexcept
        {
            if (id >= TaskGraphMaxNodesV1) [[unlikely]]
            {
                return nullptr;
            }
            return m_registry[id];
        }
        /**
         * @brief 验证槽位是否已被有效注册
         * @note 用于 Initialize 阶段的拓扑完整性校验，阻断空指针刺穿
         */
        [[nodiscard]] inline bool IsRegistered(TaskId id) const noexcept
        {
            return Resolve(id) != nullptr;
        }

        /**
         * @brief 验证注册表与外部拓扑图的匹配完整性
         * @note 在 Initialize 阶段执行一次性全量扫描，防止运行期出现未注册的 TaskId 刺穿核心
         */
        [[nodiscard]] bool Validate(const TaskGraph &graph) const noexcept;

    private:
        std::array<TaskFunction, TaskGraphMaxNodesV1> m_registry{};
    };

    // ============================================================================
    // 确定性 ABI 契约审查
    // ============================================================================
    static_assert(std::is_standard_layout_v<TaskRegistry>,
                  "FATAL: TaskRegistry must be standard layout for predictable memory representation.");

} // namespace mmo::runtime