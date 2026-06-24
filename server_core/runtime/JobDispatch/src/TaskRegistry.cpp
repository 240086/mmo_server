#include "mmo/runtime/JobDispatch/TaskRegistry.hpp"

#include "mmo/runtime/TaskGraph/TaskGraph.hpp"

namespace mmo::runtime
{

    bool TaskRegistry::Validate(
        const TaskGraph &graph) const noexcept
    {
        for (
            TaskNodeCount i = 0;
            i < graph.nodeCount;
            ++i)
        {
            const TaskNode &node =
                graph.nodes[i];

            if (!IsRegistered(node.id))
            {
                return false;
            }
        }

        return true;
    }

}