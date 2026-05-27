// /home/xj/projects/mmo_server/server_core/infrastructure/timer/include/mmo/infrastructure/timer/IClock.hpp
#pragma once

#include <mmo/infrastructure/timer/TimerTypes.hpp>

namespace mmo::infrastructure::timer {

class IClock {
public:
  virtual ~IClock() = default;

  [[nodiscard]]
  virtual TimePoint Now() const noexcept = 0;
};

} // namespace mmo::infrastructure::timer