#include "TickSchedulerTestCommon.hpp"

namespace mmo::runtime::tests
{

    TEST_F(
        TickSchedulerFixture,
        InitialStateIsCreated)
    {
        EXPECT_EQ(
            scheduler->GetState(),
            TickSchedulerState::Created);
    }

    TEST_F(
        TickSchedulerFixture,
        InitialTickIsZero)
    {
        EXPECT_EQ(
            scheduler
                ->GetRuntimeContext()
                .tickId,
            0u);
    }

}