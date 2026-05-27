#pragma once

#include <mmo/infrastructure/timer/TimerTypes.hpp>

namespace mmo::infrastructure::timer {

enum class TimerState : std::uint8_t {
  Created,
  Scheduled,
  Cancelled,
  Executing,
  Completed
};

struct TimerTask {
  TimerId id{0};

  TimePoint deadline{};

  std::uint64_t sequence{0};

  TimerCallback callback{};

  TimerState state{TimerState::Created};

  [[nodiscard]]
  bool operator>(const TimerTask &other) const noexcept {
    if (deadline != other.deadline) {
      return deadline > other.deadline;
    }

    return sequence > other.sequence;
  }
};

} // namespace mmo::infrastructure::timer