#pragma once

#include <chrono>
#include <memory>

#include <gtest/gtest.h>

#include <mmo/infrastructure/timer/IClock.hpp>
#include <mmo/infrastructure/timer/TimerQueue.hpp>
#include <mmo/runtime/RuntimePipeline/RuntimePipeline.hpp>
#include <mmo/runtime/TickScheduler/TickScheduler.hpp>

namespace mmo::runtime::tests
{

    class MockClock final
        : public infrastructure::timer::IClock
    {
    public:
        using TimePoint = infrastructure::timer::TimePoint;

        [[nodiscard]]
        TimePoint Now() const noexcept override
        {
            return now_;
        }

        void Advance(std::chrono::milliseconds delta)
        {
            now_ += delta;
        }

    private:
        TimePoint now_{};
    };

    class TickSchedulerFixture
        : public ::testing::Test
    {
    protected:
        // 🎯 核心修复 1：调整声明顺序，确保 clock 先于 timerQueue 初始化
        MockClock clock;

        // 🎯 核心修复 2：将 clock 注入到 timerQueue 中
        infrastructure::timer::TimerQueue timerQueue{clock};

        RuntimePipeline pipeline;

        TickSchedulerConfig config{std::chrono::milliseconds(0)};

        std::unique_ptr<TickScheduler> scheduler;

    protected:
        void SetUp() override
        {
            scheduler = std::make_unique<TickScheduler>(
                config,
                pipeline,
                timerQueue,
                clock);
        }
    };

} // namespace mmo::runtime::tests