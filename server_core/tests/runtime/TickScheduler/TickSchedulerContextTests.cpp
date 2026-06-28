#include "TickSchedulerTestCommon.hpp"

namespace mmo::runtime::tests
{

    TEST_F(TickSchedulerFixture, DeltaTimeMatchesConfig)
    {
        // 🎯 局部构造一个 50ms 步长的配置，用以验证时间步长映射契约
        TickSchedulerConfig customConfig{std::chrono::milliseconds(50)};

        auto customScheduler = std::make_unique<TickScheduler>(
            customConfig, pipeline, timerQueue, clock);

        customScheduler->Initialize();
        customScheduler->RunOneTick();

        EXPECT_EQ(customScheduler->GetRuntimeContext().deltaTimeMs, 50u);
    }

    TEST_F(
        TickSchedulerFixture,
        RuntimeModeBecomesRunning)
    {
        scheduler->Initialize();

        scheduler->RunOneTick();

        EXPECT_EQ(
            scheduler
                ->GetRuntimeContext()
                .mode,
            RuntimeMode::Running);
    }

    TEST_F(
        TickSchedulerFixture,
        FirstTickTransitionsToRunning)
    {
        scheduler->Initialize();

        scheduler->RunOneTick();

        EXPECT_EQ(
            scheduler->GetState(),
            TickSchedulerState::Running);
    }

}