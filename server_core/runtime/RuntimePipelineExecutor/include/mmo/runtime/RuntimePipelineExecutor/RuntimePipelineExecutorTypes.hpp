// /home/xj/projects/mmo_server/server_core/runtime/RuntimePipelineExecutor/include/mmo/runtime/RuntimePipelineExecutor/RuntimePipelineExecutorTypes.hpp
#pragma once

#include <cstdint>

namespace mmo::runtime
{

    using PhaseIndex = std::uint32_t;

    using PhaseCount = std::uint32_t;

    using PipelineExecutionId = std::uint64_t;

    enum class RuntimePipelineExecutorState : std::uint8_t
    {
        Created,
        Initialized,
        Running,
        Stopping,
        Stopped
    };

    enum class RuntimeExecutionResult : std::uint8_t
    {
        Success,
        EmptyPipeline,
        InvalidPipeline,
        DispatchFailure,
        Cancelled
    };

} // namespace mmo::runtime