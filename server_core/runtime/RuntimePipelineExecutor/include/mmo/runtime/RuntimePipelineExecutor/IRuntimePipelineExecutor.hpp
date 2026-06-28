// /home/xj/projects/mmo_server/server_core/runtime/RuntimePipelineExecutor/include/mmo/runtime/RuntimePipelineExecutor/IRuntimePipelineExecutor.hpp
#pragma once

#include <mmo/runtime/RuntimePipelineExecutor/RuntimePipelineExecutorTypes.hpp>

namespace mmo::runtime
{

    class IRuntimePipelineExecutor
    {
    public:
        virtual ~IRuntimePipelineExecutor() = default;

        virtual bool Initialize() = 0;

        virtual void Shutdown() = 0;

        [[nodiscard]]
        virtual RuntimeExecutionResult
        ExecutePipeline() = 0;

        [[nodiscard]]
        virtual RuntimePipelineExecutorState
        GetState() const noexcept = 0;
    };

} // namespace mmo::runtime