#pragma once

#include <mmo/infrastructure/timer/IClock.hpp>

namespace mmo::infrastructure::timer {

class SteadyClock final : public IClock {
public:
  [[nodiscard]]
  TimePoint Now() const noexcept override;
};

} // namespace mmo::infrastructure::timer