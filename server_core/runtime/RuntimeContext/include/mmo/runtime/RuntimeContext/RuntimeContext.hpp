// /home/xj/projects/mmo_server/server_core/runtime/RuntimeContext/include/mmo/runtime/RuntimeContext/RuntimeContext.hpp
#pragma once

#include <mmo/runtime/RuntimeContext/RuntimeMode.hpp>
#include <mmo/runtime/RuntimeContext/RuntimeTypes.hpp>

namespace mmo::runtime
{

    struct RuntimeContext final
    {
        TickId tickId{0};

        FrameIndex frameIndex{0};

        TickDurationMs deltaTimeMs{0};

        RuntimeMode mode{RuntimeMode::Initializing};

        WorkerCount workerCount{0};
    };

} // namespace mmo::runtime