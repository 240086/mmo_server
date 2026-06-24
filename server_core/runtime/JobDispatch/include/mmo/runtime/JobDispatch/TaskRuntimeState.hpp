// /home/xj/projects/mmo_server/server_core/runtime/JobDispatch/include/mmo/runtime/JobDispatch/TaskRuntimeState.hpp
#pragma once

#include <atomic>
#include <cstdint>
#include <type_traits>
#include "mmo/runtime/JobDispatch/JobDispatchConstants.hpp"

namespace mmo::runtime
{

    /**
     * @brief 执行态 mutable 核心状态机
     * @note 彻底隔离至独立缓存行，保障多工作线程并发递减剩余入度计数时绝无 Cache 颠簸
     */
    struct alignas(kCacheLineSize) TaskRuntimeState final
    {
        std::atomic<std::uint32_t> remainingDependencies{0};
    };

    // ============================================================================
    // 确定性 ABI 契约审查
    // ============================================================================
    static_assert(std::is_standard_layout_v<TaskRuntimeState>,
                  "FATAL: TaskRuntimeState must comply with Standard Layout.");

    static_assert(std::is_trivially_destructible_v<TaskRuntimeState>,
                  "FATAL: TaskRuntimeState must be Trivially Destructible to avoid pipeline stall.");

    static_assert(alignof(TaskRuntimeState) == kCacheLineSize,
                  "FATAL: TaskRuntimeState missing explicit cache-line isolation alignment.");

} // namespace mmo::runtime