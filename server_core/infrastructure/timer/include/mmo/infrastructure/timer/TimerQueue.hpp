// /home/xj/projects/mmo_server/server_core/infrastructure/timer/include/mmo/infrastructure/timer/TimerQueue.hpp
#pragma once

#include <atomic>
#include <queue>
#include <unordered_set>
#include <vector>
#include <mutex>

#include <mmo/infrastructure/timer/TimerTask.hpp>

namespace mmo::infrastructure::timer {

class TimerQueue {
public:
  TimerQueue() = default;
  TimerQueue(const TimerQueue &) = delete;
  TimerQueue &operator=(const TimerQueue &) = delete;

public:
  [[nodiscard]]
  TimerId Schedule(Duration delay, TimerCallback callback);

  [[nodiscard]]
  bool Cancel(TimerId timerId);

  void Tick(TimePoint now);

  [[nodiscard]]
  std::size_t Size() const noexcept;

private:
  void DispatchExpired(TimePoint now);

  [[nodiscard]]
  bool IsCancelled(TimerId timerId) const;

private:
  std::priority_queue<TimerTask, std::vector<TimerTask>, std::greater<>> tasks_;

  std::unordered_set<TimerId> cancelledTimers_;

  std::atomic<TimerId> nextId_{1};

  mutable std::mutex mutex_;

  TimerSequence nextSequence_{0};
};

} // namespace mmo::infrastructure::timer