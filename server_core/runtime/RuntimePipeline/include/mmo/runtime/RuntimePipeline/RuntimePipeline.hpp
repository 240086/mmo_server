// /home/xj/projects/mmo_server/server_core/runtime/RuntimePipeline/include/mmo/runtime/RuntimePipeline/RuntimePipeline.hpp
#pragma once

#include <array>

#include "mmo/runtime/RuntimePhase/RuntimePhase.hpp"
#include "mmo/runtime/RuntimePhase/RuntimePhaseConstants.hpp"

namespace mmo::runtime
{

    struct RuntimePipeline final
    {
        std::array<
            RuntimePhase,
            RuntimePhaseCountV1>
            phases{
                RuntimePhase{RuntimePhaseId::Input},
                RuntimePhase{RuntimePhaseId::Simulation},
                RuntimePhase{RuntimePhaseId::PostSimulation},
                RuntimePhase{RuntimePhaseId::Output}};

        // ============================================================================
        // 容器解耦接口 (Storage Encapsulation)
        // ============================================================================
        [[nodiscard]] auto begin() noexcept { return phases.begin(); }
        [[nodiscard]] auto end() noexcept { return phases.end(); }
        [[nodiscard]] auto begin() const noexcept { return phases.begin(); }
        [[nodiscard]] auto end() const noexcept { return phases.end(); }

        [[nodiscard]] std::size_t size() const noexcept { return phases.size(); }
    };

}