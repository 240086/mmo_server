#include "mmo/runtime/TickScheduler/TickScheduler.hpp"

#include <stdexcept>
#include <thread>

#include "mmo/infrastructure/timer/IClock.hpp"
#include "mmo/infrastructure/timer/TimerQueue.hpp"
#include "mmo/runtime/RuntimePipeline/RuntimePipeline.hpp"

namespace mmo::runtime
{

    TickScheduler::TickScheduler(
        TickSchedulerConfig config,
        RuntimePipeline &pipeline,
        infrastructure::timer::TimerQueue &timerQueue,
        infrastructure::timer::IClock &clock)
        : m_config(config),
          m_pipeline(pipeline),
          m_timerQueue(timerQueue),
          m_clock(clock)
    {
    }

    bool TickScheduler::Initialize()
    {
        if (m_state != TickSchedulerState::Created)
        {
            throw std::logic_error(
                "TickScheduler already initialized");
        }

        m_context.tickId = 0;

        UpdateRuntimeContext();

        m_context.mode = RuntimeMode::Initializing;
        m_context.workerCount = 0;

        m_stopRequested.store(
            false,
            std::memory_order_release);

        // 🎯 建立初始时间锚点
        m_nextTickTime = m_clock.Now() + m_config.tickInterval;

        m_state = TickSchedulerState::Initialized;

        return true;
    }

    void TickScheduler::Shutdown()
    {
        if (m_state == TickSchedulerState::Stopped)
        {
            return;
        }

        m_stopRequested.store(
            true,
            std::memory_order_release);

        m_context.mode = RuntimeMode::Stopping;
        m_state = TickSchedulerState::Stopping;

        m_context.mode = RuntimeMode::Stopped;
        m_state = TickSchedulerState::Stopped;
    }

    void TickScheduler::RunOneTick()
    {
        if (m_state == TickSchedulerState::Stopped)
        {
            throw std::logic_error("RunOneTick after shutdown");
        }

        if (m_state == TickSchedulerState::Created)
        {
            throw std::logic_error("RunOneTick before initialize");
        }

        m_context.mode = RuntimeMode::Running;

        if (m_state == TickSchedulerState::Initialized)
        {
            m_state = TickSchedulerState::Running;
        }

        // 🎯 1. 架构级时序重构：优先捕获当前逻辑帧的绝对采样点
        // 确保本帧生命周期内的所有数据结构（如 TimerQueue）共享相同的确定性时空基准
        const auto now = m_clock.Now();
        m_timerQueue.Tick(now);

        // 🎯 2. 更新上下文数据状态
        UpdateRuntimeContext();

        // 🎯 3. 分发并执行拓扑管线
        DispatchPipeline();

        // 🎯 4. 推进逻辑帧计数器
        AdvanceTick();

        // 🎯 5. 帧率限制器移至帧尾，完成当前帧控制权交付并锁定下一帧期望时刻
        WaitNextTick();
    }

    void TickScheduler::RunUntilStopped()
    {
        if (m_state == TickSchedulerState::Stopped)
        {
            throw std::logic_error("RunUntilStopped after shutdown");
        }

        while (!m_stopRequested.load(std::memory_order_acquire))
        {
            RunOneTick();
        }
    }

    TickSchedulerState TickScheduler::GetState() const noexcept
    {
        return m_state;
    }

    const RuntimeContext &TickScheduler::GetRuntimeContext() const noexcept
    {
        return m_context;
    }

    void TickScheduler::WaitNextTick()
    {
        std::this_thread::sleep_until(m_nextTickTime);

        m_nextTickTime += m_config.tickInterval;
    }

    void TickScheduler::UpdateRuntimeContext()
    {
        m_context.deltaTimeMs = static_cast<TickDurationMs>(
            std::chrono::duration_cast<std::chrono::milliseconds>(m_config.tickInterval).count());
    }

    void TickScheduler::AdvanceTick() noexcept
    {
        ++m_context.tickId;
    }

    void TickScheduler::DispatchPipeline()
    {
        for (const auto &phase : m_pipeline.phases)
        {
            (void)phase;
        }
    }

} // namespace mmo::runtime