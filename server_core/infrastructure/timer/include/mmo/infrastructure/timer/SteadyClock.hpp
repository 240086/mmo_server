// /home/xj/projects/mmo_server/server_core/infrastructure/timer/include/mmo/infrastructure/timer/SteadyClock.hpp
#pragma once

#include <mmo/infrastructure/timer/IClock.hpp>

namespace mmo::infrastructure::timer {

class SteadyClock final : public IClock {
public:
  [[nodiscard]]
  TimePoint Now() const noexcept override;
};

} // namespace mmo::infrastructure::timer