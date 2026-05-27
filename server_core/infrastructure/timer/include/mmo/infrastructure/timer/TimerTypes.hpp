// /home/xj/projects/mmo_server/server_core/infrastructure/timer/include/mmo/infrastructure/timer/TimerTypes.hpp
#pragma once

#include <chrono>
#include <cstdint>
#include <functional>

namespace mmo::infrastructure::timer {

using Clock = std::chrono::steady_clock;

using TimePoint = Clock::time_point;

using Duration = std::chrono::milliseconds;

using Nanoseconds = std::chrono::nanoseconds;

using TimerId = std::uint64_t;

using TimerSequence = std::uint64_t;

using TimerCallback = std::function<void()>;

} // namespace mmo::infrastructure::timer