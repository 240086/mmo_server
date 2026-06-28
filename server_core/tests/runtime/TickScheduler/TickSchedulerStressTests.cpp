#include "TickSchedulerTestCommon.hpp"

namespace mmo::runtime::tests
{

    TEST_F(
        TickSchedulerFixture,
        LongRunTickStability)
    {
        scheduler->Initialize();

        constexpr std::uint64_t TickCount =
            100000;

        for (std::uint64_t i = 0;
             i < TickCount;
             ++i)
        {
            scheduler->RunOneTick();
        }

        EXPECT_EQ(
            scheduler
                ->GetRuntimeContext()
                .tickId,
            TickCount);
    }

}