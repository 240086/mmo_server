#include "TickSchedulerTestCommon.hpp"

namespace mmo::runtime::tests
{

    TEST_F(
        TickSchedulerFixture,
        SingleTickAdvancesTickId)
    {
        scheduler->Initialize();

        scheduler->RunOneTick();

        EXPECT_EQ(
            scheduler
                ->GetRuntimeContext()
                .tickId,
            1u);
    }

    TEST_F(
        TickSchedulerFixture,
        MultipleTicksAdvanceMonotonically)
    {
        scheduler->Initialize();

        for (std::size_t i = 0; i < 100; ++i)
        {
            scheduler->RunOneTick();
        }

        EXPECT_EQ(
            scheduler
                ->GetRuntimeContext()
                .tickId,
            100u);
    }

}