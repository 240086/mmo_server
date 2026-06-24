// /home/xj/projects/mmo_server/server_core/runtime/TaskGraph/include/mmo/runtime/TaskGraph/TaskNode.hpp
#pragma once

#include <cstdint>

#include "mmo/runtime/RuntimePhase/RuntimePhaseId.hpp"

#include "mmo/runtime/TaskGraph/TaskGraphTypes.hpp"

namespace mmo::runtime
{

    class TaskNode final
    {
        TaskId id{InvalidTaskId};                      // 任务唯一标识 ID
        RuntimePhaseId phaseId{RuntimePhaseId::Input}; // 所属的运行时 Phase ID
        std::uint32_t dependencyOffset{0};             // 该节点在 CSR 边表中的起始相邻边偏移量
        std::uint32_t childCount{0};                   // 出度 (Out-degree)：后继子节点的数量
        std::uint32_t parentCount{0};                  // 入度 (In-degree)：前置依赖父节点的数量（方案A核心核心）
    };

}