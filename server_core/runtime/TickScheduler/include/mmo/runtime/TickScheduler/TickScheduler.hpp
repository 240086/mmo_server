// /home/xj/projects/mmo_server/server_core/runtime/TickScheduler/include/mmo/runtime/TickScheduler/TickScheduler.hpp
#pragma once
#include <atomic>

#include "mmo/runtime/TickScheduler/ITickScheduler.hpp"
#include "mmo/runtime/TickScheduler/TickSchedulerConfig.hpp"
#include <mmo/infrastructure/timer/TimerTypes.hpp>
#include <mmo/runtime/RuntimeContext/RuntimeContext.hpp>

namespace mmo::infrastructure::timer
{
    class IClock;
    class TimerQueue;
}

namespace mmo::runtime
{
    class RuntimePipeline;

    class TickScheduler final
        : public ITickScheduler
    {
    public:
        TickScheduler(
            TickSchedulerConfig config,
            RuntimePipeline &pipeline,
            infrastructure::timer::TimerQueue &timerQueue,
            infrastructure::timer::IClock &clock);

        ~TickScheduler() override = default;

        TickScheduler(
            const TickScheduler &) = delete;

        TickScheduler &
        operator=(
            const TickScheduler &) = delete;

        TickScheduler(
            TickScheduler &&) = delete;

        TickScheduler &
        operator=(
            TickScheduler &&) = delete;

    public:
        bool Initialize() override;

        void Shutdown() override;

        void RunOneTick() override;

        void RunUntilStopped() override;

        [[nodiscard]]
        TickSchedulerState
        GetState() const noexcept override;

        [[nodiscard]]
        const RuntimeContext &
        GetRuntimeContext() const noexcept override;

    private:
        void WaitNextTick();

        void UpdateRuntimeContext();

        void AdvanceTick() noexcept;

        void DispatchPipeline();

    private:
        TickSchedulerConfig m_config;

        RuntimeContext m_context;

        RuntimePipeline &m_pipeline;

        infrastructure::timer::TimerQueue &m_timerQueue;

        infrastructure::timer::IClock &m_clock;

        TickSchedulerState m_state{
            TickSchedulerState::Created};

        infrastructure::timer::TimePoint
            m_nextTickTime{};

        std::atomic<bool> m_stopRequested{false};
    };

} // namespace mmo::runtime