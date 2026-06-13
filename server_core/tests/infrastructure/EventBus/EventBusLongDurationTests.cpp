// server_core/tests/infrastructure/EventBus/EventBusLongDurationTests.cpp
#include <gtest/gtest.h>
#include <atomic>
#include <mmo/infrastructure/EventBus/EventBus.hpp>

using namespace mmo::infrastructure::event_bus;

namespace
{
    struct LongEvent
    {
        uint64_t value;
    };
}

TEST(EventBusLongDurationTests, MillionDispatchesStableVerification)
{
    EventBus bus;
    std::atomic<uint64_t> count{0};

    bus.Subscribe<LongEvent>([&](const LongEvent &)
                             { count.fetch_add(1, std::memory_order_relaxed); });

    constexpr uint64_t N = 1000000;
    for (uint64_t i = 0; i < N; ++i)
    {
        bus.Publish(LongEvent{i});
    }

    EXPECT_EQ(count.load(), N);
}

TEST(EventBusLongDurationTests, MassiveSubscribeUnsubscribeLifecycleCycles)
{
    EventBus bus;
    constexpr int N = 50000;
    std::atomic<int> anchorCount{0};

    // 注册一个长周期锚点，确保存储 Map 始终处于热激活状态
    auto anchorToken = bus.Subscribe<LongEvent>([&](const LongEvent &)
                                                { anchorCount.fetch_add(1, std::memory_order_relaxed); });

    // 狂暴循环：创建、注销、揉碎并回收生命周期
    for (int i = 0; i < N; ++i)
    {
        auto token = bus.Subscribe<LongEvent>([](const LongEvent &) {});
        bus.Unsubscribe(token);
    }

    // 最终验证：经历大规模拓扑废弃和墓碑堆积后，原有的核心链路依然保持完好通畅
    bus.Publish(LongEvent{});
    EXPECT_EQ(anchorCount.load(), 1);

    bus.Unsubscribe(anchorToken);
}