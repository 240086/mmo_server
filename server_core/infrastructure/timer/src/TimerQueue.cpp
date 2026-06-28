#include <mmo/infrastructure/timer/TimerQueue.hpp>

namespace mmo::infrastructure::timer
{
  TimerQueue::TimerQueue(
      IClock &clock)
      : m_clock(clock)
  {
  }

  TimerId TimerQueue::Schedule(Duration delay, TimerCallback callback)
  {
    if (!callback)
    {
      return InvalidTimerId;
    }

    const TimerId timerId = nextId_.fetch_add(1, std::memory_order_relaxed);

    TimerTask task;

    task.id = timerId;

    task.deadline = m_clock.Now() + delay;

    task.state = TimerState::Scheduled;

    task.callback = std::move(callback);

    task.sequence =
        nextSequence_.fetch_add(
            1,
            std::memory_order_relaxed);

    {
      std::scoped_lock lock(mutex_);

      tasks_.push(std::move(task));
    }

    return timerId;
  }

  bool TimerQueue::Cancel(TimerId timerId)
  {
    std::scoped_lock lock(mutex_);

    return cancelledTimers_.insert(timerId).second;
  }

  void TimerQueue::Tick(TimePoint now) { DispatchExpired(now); }

  std::size_t TimerQueue::Size() const noexcept
  {
    std::scoped_lock lock(mutex_);

    return tasks_.size();
  }

  bool TimerQueue::IsCancelled(
      TimerId timerId) const
  {
    std::scoped_lock lock(
        mutex_);

    return cancelledTimers_.contains(
        timerId);
  }

  void TimerQueue::DispatchExpired(TimePoint now)
  {
    // Never execute callbacks
    // while holding mutex.
    for (;;)
    {
      TimerTask task;

      {
        std::scoped_lock lock(mutex_);

        if (tasks_.empty())
        {
          return;
        }

        if (tasks_.top().deadline > now)
        {
          return;
        }

        task = tasks_.top();
        tasks_.pop();
      }

      {
        std::scoped_lock lock(mutex_);

        auto iter =
            cancelledTimers_.find(task.id);

        if (iter != cancelledTimers_.end())
        {
          cancelledTimers_.erase(iter);

          continue;
        }
      }

      task.state = TimerState::Executing;

      if (task.callback)
      {
        task.callback();
      }

      task.state = TimerState::Completed;
    }
  }

} // namespace mmo::infrastructure::timer