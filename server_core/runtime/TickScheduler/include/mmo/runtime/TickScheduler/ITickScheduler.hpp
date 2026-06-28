// /home/xj/projects/mmo_server/server_core/runtime/TickScheduler/include/mmo/runtime/TickScheduler/ITickScheduler.hpp
#pragma once

#include "mmo/runtime/TickScheduler/TickSchedulerTypes.hpp"

namespace mmo::runtime
{

    class RuntimeContext;

    class ITickScheduler
    {
    public:
        virtual ~ITickScheduler() = default;

        virtual bool Initialize() = 0;

        virtual void Shutdown() = 0;

        virtual void RunOneTick() = 0;

        virtual void RunUntilStopped() = 0;

        [[nodiscard]]
        virtual TickSchedulerState
        GetState() const noexcept = 0;

        [[nodiscard]]
        virtual const RuntimeContext &
        GetRuntimeContext() const noexcept = 0;
    };

} // namespace mmo::runtime