#include <mmo/infrastructure/timer/SteadyClock.hpp>

namespace mmo::infrastructure::timer {

TimePoint SteadyClock::Now() const noexcept { return Clock::now(); }

} // namespace mmo::infrastructure::timer