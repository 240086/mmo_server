// /home/xj/projects/mmo_server/server_core/runtime/JobDispatch/include/mmo/runtime/JobDispatch/TaskFunction.hpp
#pragma once

#include <mmo/runtime/RuntimeContext/RuntimeContext.hpp>

namespace mmo::runtime
{

    using TaskFunction =
        void (*)(RuntimeContext &) noexcept;

}