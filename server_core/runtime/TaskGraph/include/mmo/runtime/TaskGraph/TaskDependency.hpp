// /home/xj/projects/mmo_server/server_core/runtime/TaskGraph/include/mmo/runtime/TaskGraph/TaskDependency.hpp
#pragma once

#include "mmo/runtime/TaskGraph/TaskGraphTypes.hpp"

namespace mmo::runtime
{

    struct TaskDependency final
    {
        TaskId parentTaskId{InvalidTaskId};

        TaskId childTaskId{InvalidTaskId};
    };

}