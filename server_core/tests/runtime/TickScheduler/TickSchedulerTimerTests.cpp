#include "TickSchedulerTestCommon.hpp"

namespace mmo::runtime::tests
{

    TEST_F(TickSchedulerFixture, ExpiredTimerExecutes)
    {
        bool fired = false;

        // 1. 初始化调度器，锚定时间
        scheduler->Initialize();

        // 2. 注册 0ms 延迟定时器。
        // 此时它内部算出的 task.deadline = clock.Now() + 0 = 0
        (void)timerQueue.Schedule(
            std::chrono::milliseconds(0),
            [&]
            { fired = true; });

        // 3. 显式推进 Mock 时钟 1 毫秒
        // 此时 clock 内部的 now_ 变成 1
        clock.Advance(std::chrono::milliseconds(1));

        // 4. 执行滴答
        // 此时 RunOneTick 内部采样最新的 now 得到 1，传给 m_timerQueue.Tick(1)
        // 1 >= 0，小顶堆条件被完美跨越！
        scheduler->RunOneTick();

        // 🏁 终极断言
        EXPECT_TRUE(fired);
    }

} // namespace mmo::runtime::tests