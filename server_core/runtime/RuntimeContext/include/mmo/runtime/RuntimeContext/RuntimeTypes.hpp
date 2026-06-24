// /home/xj/projects/mmo_server/server_core/runtime/RuntimeContext/include/mmo/runtime/RuntimeContext/RuntimeTypes.hpp
#pragma once

#include <cstdint>

namespace mmo::runtime
{

    using TickId = std::uint64_t;

    using FrameIndex = std::uint64_t;

    using TickDurationMs = std::uint32_t;

    using WorkerCount = std::uint32_t;

} // namespace mmo::runtime