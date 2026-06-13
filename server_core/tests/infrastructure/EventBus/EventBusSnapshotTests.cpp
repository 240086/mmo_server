// server_core/tests/infrastructure/EventBus/EventBusSnapshotTests.cpp
#include <gtest/gtest.h>
#include <atomic>
#include <mmo/infrastructure/EventBus/EventBus.hpp>

using namespace mmo::infrastructure::event_bus;

namespace
{
    struct SnapshotEvent
    {
        int value;
    };
}

// 铁证测试：验证 Publish 消费的是触发瞬间的纯净快照，回调内新追加的订阅者绝不越界污染本次交互
TEST(EventBusSnapshotTests, MutateTopologyDuringPublishIsolation)
{
    EventBus bus;
    std::atomic<int> innerCounter{0};
    SubscriptionToken tokenDynamic;

    // 订阅者 A
    bus.Subscribe<SnapshotEvent>([&](const SnapshotEvent &)
                                 {
        // 在回调热路径内部，疯狂注入全新订阅者 B
        tokenDynamic = bus.Subscribe<SnapshotEvent>([&](const SnapshotEvent&) {
            innerCounter.fetch_add(1, std::memory_order_relaxed);
        }); });

    // 第一次发布：此时快照只有 [A]。A 执行时把 B 塞进底层系统，但当前迭代快照隔离，B 绝不能执行。
    bus.Publish(SnapshotEvent{});
    EXPECT_EQ(innerCounter.load(), 0); // 核心断言：快照隔离成功，B 本轮静默！

    // 第二次发布：此时快照包含 [A, B]，B 必须复活执行
    bus.Publish(SnapshotEvent{});
    EXPECT_EQ(innerCounter.load(), 1);

    bus.Unsubscribe(tokenDynamic);
}

TEST(EventBusSnapshotTests, ClearRemovesAllHandlers)
{
    EventBus bus;
    std::atomic<int> counter{0};

    bus.Subscribe<SnapshotEvent>([&](const SnapshotEvent &)
                                 { counter.fetch_add(1, std::memory_order_relaxed); });

    bus.Publish(SnapshotEvent{});
    EXPECT_EQ(counter.load(), 1);

    bus.Clear(); // 斩断所有拓扑

    bus.Publish(SnapshotEvent{});
    EXPECT_EQ(counter.load(), 1); // 稳态不变，证明清理彻底
}