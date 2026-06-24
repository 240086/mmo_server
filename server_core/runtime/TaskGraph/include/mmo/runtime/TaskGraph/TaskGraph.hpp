// /home/xj/projects/mmo_server/server_core/runtime/TaskGraph/include/mmo/runtime/TaskGraph/TaskGraph.hpp
#pragma once

#include <array>

#include "mmo/runtime/TaskGraph/TaskNode.hpp"

#include "mmo/runtime/TaskGraph/TaskGraphConstants.hpp"
#include <mmo/runtime/RuntimePhase/RuntimePhaseConstants.hpp>

namespace mmo::runtime
{
    struct PhaseRange final
    {
        std::uint16_t begin{0};

        std::uint16_t count{0};
    };

    struct TaskGraph final
    {
        std::array<
            TaskNode,
            TaskGraphMaxNodesV1>
            nodes{};

        TaskNodeCount nodeCount{0};

        std::array<
            TaskId,
            TaskGraphMaxDependenciesV1>
            childTaskIds{};

        std::array<
            PhaseRange,
            RuntimePhaseCountV1>
            phaseRanges{};

        std::array<
            TaskId,
            TaskGraphMaxNodesV1>
            taskIdToNodeIndex{};

        TaskDependencyCount dependencyCount{0};
    };

}