// /home/xj/projects/mmo_server/server_core/tests/runtime/JobDispatch/JobDispatchTestCommon.hpp
#pragma once

#include <atomic>
#include <vector>

#include <gtest/gtest.h>

#include "mmo/runtime/JobDispatch/TaskRegistry.hpp"
#include "mmo/runtime/TaskGraph/TaskGraph.hpp"

namespace mmo::runtime::test
{

    struct DummyRuntimeContext
    {
    };

    struct ExecutionRecorder
    {
        std::mutex mutex;
        std::vector<TaskId> executionOrder;

        void Record(TaskId id)
        {
            std::scoped_lock lock(mutex);
            executionOrder.push_back(id);
        }
    };

    inline TaskGraph CreateSingleNodeGraph()
    {
        TaskGraph graph;

        graph.nodeCount = 1;

        graph.nodes[0].id = 0;
        graph.nodes[0].phaseId = RuntimePhaseId::Input;
        graph.nodes[0].parentCount = 0;
        graph.nodes[0].childCount = 0;

        graph.phaseRanges[static_cast<std::size_t>(
            RuntimePhaseId::Input)] =
            {
                0,
                1};

        graph.taskIdToNodeIndex[0] = 0;

        return graph;
    }

    inline TaskGraph CreateLinearGraph()
    {
        TaskGraph graph;

        graph.nodeCount = 3;

        graph.nodes[0].id = 0;
        graph.nodes[0].parentCount = 0;
        graph.nodes[0].childCount = 1;
        graph.nodes[0].dependencyOffset = 0;
        graph.nodes[0].phaseId = RuntimePhaseId::Input;

        graph.nodes[1].id = 1;
        graph.nodes[1].parentCount = 1;
        graph.nodes[1].childCount = 1;
        graph.nodes[1].dependencyOffset = 1;
        graph.nodes[1].phaseId = RuntimePhaseId::Input;

        graph.nodes[2].id = 2;
        graph.nodes[2].parentCount = 1;
        graph.nodes[2].childCount = 0;
        graph.nodes[2].phaseId = RuntimePhaseId::Input;

        graph.childTaskIds[0] = 1;
        graph.childTaskIds[1] = 2;

        graph.phaseRanges[static_cast<std::size_t>(
            RuntimePhaseId::Input)] =
            {
                0,
                3};

        graph.taskIdToNodeIndex[0] = 0;
        graph.taskIdToNodeIndex[1] = 1;
        graph.taskIdToNodeIndex[2] = 2;

        return graph;
    }

}