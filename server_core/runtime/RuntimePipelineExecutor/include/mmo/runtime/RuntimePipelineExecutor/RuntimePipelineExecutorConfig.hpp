// /home/xj/projects/mmo_server/server_core/runtime/RuntimePipelineExecutor/include/mmo/runtime/RuntimePipelineExecutor/RuntimePipelineExecutorConfig.hpp
#pragma once

namespace mmo::runtime
{

    struct RuntimePipelineExecutorConfig
    {
        bool failFast{true};

        bool enableProfiling{false};

        bool collectMetrics{false};
    };

} // namespace mmo::runtime