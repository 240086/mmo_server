#include <mmo/infrastructure/timer/TimerQueue.hpp>

namespace mmo::infrastructure::timer {

TimerId TimerQueue::Schedule(Duration delay, TimerCallback callback) {
  if (!callback) {
    return 0;
  }

  const TimerId timerId = nextId_.fetch_add(1, std::memory_order_relaxed);

  TimerTask task;

  task.id = timerId;

  task.deadline = Clock::now() + delay;

  task.sequence = nextSequence_++;

  task.state = TimerState::Scheduled;

  task.callback = std::move(callback);

  {
    std::scoped_lock lock(mutex_);

    tasks_.push(std::move(task));
  }

  return timerId;
}

bool TimerQueue::Cancel(TimerId timerId) {
  std::scoped_lock lock(mutex_);

  return cancelledTimers_.insert(timerId).second;
}

void TimerQueue::Tick(TimePoint now) { DispatchExpired(now); }

std::size_t TimerQueue::Size() const noexcept {
  std::scoped_lock lock(mutex_);

  return tasks_.size();
}

bool TimerQueue::IsCancelled(TimerId timerId) const {
  return cancelledTimers_.contains(timerId);
}

void TimerQueue::DispatchExpired(TimePoint now) {
  for (;;) {
    TimerTask task;

    {
      std::scoped_lock lock(mutex_);

      if (tasks_.empty()) {
        return;
      }

      if (tasks_.top().deadline > now) {
        return;
      }

      task = std::move(const_cast<TimerTask &>(tasks_.top()));

      tasks_.pop();
    }

    if (IsCancelled(task.id)) {
      std::scoped_lock lock(mutex_);

      cancelledTimers_.erase(task.id);

      continue;
    }

    task.state = TimerState::Executing;

    if (task.callback) {
      task.callback();
    }

    task.state = TimerState::Completed;
  }
}

} // namespace mmo::infrastructure::timer