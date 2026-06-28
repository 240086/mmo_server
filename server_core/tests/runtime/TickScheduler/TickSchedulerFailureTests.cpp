#include "TickSchedulerTestCommon.hpp"

namespace mmo::runtime::tests
{

    TEST_F(
        TickSchedulerFixture,
        RunBeforeInitializeThrows)
    {
        EXPECT_THROW(
            scheduler->RunOneTick(),
            std::logic_error);
    }

    TEST_F(
        TickSchedulerFixture,
        DoubleInitializeThrows)
    {
        scheduler->Initialize();

        EXPECT_THROW(
            scheduler->Initialize(),
            std::logic_error);
    }

}