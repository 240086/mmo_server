#include "TickSchedulerTestCommon.hpp"

namespace mmo::runtime::tests
{

    TEST_F(
        TickSchedulerFixture,
        InitializeChangesState)
    {
        scheduler->Initialize();

        EXPECT_EQ(
            scheduler->GetState(),
            TickSchedulerState::Initialized);
    }

    TEST_F(
        TickSchedulerFixture,
        ShutdownTransitionsToStopped)
    {
        scheduler->Initialize();

        scheduler->Shutdown();

        EXPECT_EQ(
            scheduler->GetState(),
            TickSchedulerState::Stopped);

        EXPECT_EQ(
            scheduler
                ->GetRuntimeContext()
                .mode,
            RuntimeMode::Stopped);
    }

    TEST_F(
        TickSchedulerFixture,
        RunOneTickTransitionsToRunning)
    {
        scheduler->Initialize();

        scheduler->RunOneTick();

        EXPECT_EQ(
            scheduler->GetState(),
            TickSchedulerState::Running);

        EXPECT_EQ(
            scheduler
                ->GetRuntimeContext()
                .mode,
            RuntimeMode::Running);
    }

}