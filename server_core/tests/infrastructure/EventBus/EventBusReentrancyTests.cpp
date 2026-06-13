// server_core/tests/infrastructure/EventBus/EventBusReentrancyTests.cpp
#include <gtest/gtest.h>
#include <atomic>
#include <mmo/infrastructure/EventBus/EventBus.hpp>

using namespace mmo::infrastructure::event_bus;

namespace
{
    struct TestEvent
    {
        int value;
    };
}

// 验证：订阅者在执行过程中注销自己，不会引发无序迭代器失效或野指针踩空
TEST(EventBusReentrancyTests, SelfUnsubscribe)
{
    EventBus bus;
    std::atomic<int> counter{0};
    SubscriptionToken token;

    token = bus.Subscribe<TestEvent>([&](const TestEvent &)
                                     {
                                         counter.fetch_add(1, std::memory_order_relaxed);
                                         bus.Unsubscribe(token); // 拓扑自杀
                                     });

    bus.Publish(TestEvent{});
    EXPECT_EQ(counter.load(), 1);

    bus.Publish(TestEvent{});
    EXPECT_EQ(counter.load(), 1); // 第二次绝不触发
}

// 验证：相互注销时的确定性原子抑制行为
TEST(EventBusReentrancyTests, CrossUnsubscribeDeterministicBehavior)
{
    EventBus bus;
    std::atomic<int> execCountA{0};
    std::atomic<int> execCountB{0};

    SubscriptionToken tokenA;
    SubscriptionToken tokenB;

    // 同时订阅，故意制造互砍（重入注销）场景
    tokenA = bus.Subscribe<TestEvent>([&](const TestEvent &)
                                      {
                                          execCountA.fetch_add(1, std::memory_order_relaxed);
                                          bus.Unsubscribe(tokenB); // A 试图注销 B
                                      });

    tokenB = bus.Subscribe<TestEvent>([&](const TestEvent &)
                                      {
                                          execCountB.fetch_add(1, std::memory_order_relaxed);
                                          bus.Unsubscribe(tokenA); // B 试图注销 A
                                      });

    // 第一次发布
    bus.Publish(TestEvent{});
    int totalFirstRound = execCountA.load() + execCountB.load();

    // 无论是 1（一方实时扼杀了另一方）还是 2（快照完全隔离执行），都是安全的，绝不能引发 Crash 或死锁
    EXPECT_TRUE(totalFirstRound == 1 || totalFirstRound == 2);

    // 第二次发布
    bus.Publish(TestEvent{});
    int totalSecondRound = execCountA.load() + execCountB.load();

    // 【终极数学不变性断言】
    // 无论是情况 A (1 + 1 = 2) 还是情况 B (2 + 0 = 2)，两轮风暴过后，总计数必须稳稳收口在 2！
    // 这铁证了被注销的节点在第二轮绝不会复活，且重入注销逻辑未破坏内存结构。
    EXPECT_EQ(totalSecondRound, 2)
        << "Cross unsubscribe failed invariant! First round: " << totalFirstRound
        << ", Second round total: " << totalSecondRound;
}