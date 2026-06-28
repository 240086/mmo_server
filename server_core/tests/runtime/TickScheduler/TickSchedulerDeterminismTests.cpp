#include "TickSchedulerTestCommon.hpp"

namespace mmo::runtime::tests
{

    TEST_F(
        TickSchedulerFixture,
        TickIdMonotonicIncrease)
    {
        scheduler->Initialize();

        TickId previous = 0;

        constexpr std::size_t TickCount =
            10000;

        for (std::size_t i = 0;
             i < TickCount;
             ++i)
        {
            scheduler->RunOneTick();

            const TickId current =
                scheduler
                    ->GetRuntimeContext()
                    .tickId;

            EXPECT_EQ(
                current,
                previous + 1);

            previous =
                current;
        }
    }

}