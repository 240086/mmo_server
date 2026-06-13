// server_core/tests/infrastructure/EventBus/EventBusConcurrencyTests.cpp
#include <gtest/gtest.h>
#include <atomic>
#include <thread>
#include <vector>
#include <mmo/infrastructure/EventBus/EventBus.hpp>

using namespace mmo::infrastructure::event_bus;

namespace
{
    struct ConcurrentEvent
    {
        int value;
    };
}

TEST(EventBusConcurrencyTests, ConcurrentPublishAndMutationCorrectness)
{
    EventBus bus;
    std::atomic<bool> running{true};

    std::atomic<uint64_t> totalPublished{0};
    std::atomic<uint64_t> permanentConsumed{0};

    // 建立一个常驻订阅者作为逻辑锚点（数学不变性）
    auto permanentToken = bus.Subscribe<ConcurrentEvent>([&](const ConcurrentEvent &)
                                                         { permanentConsumed.fetch_add(1, std::memory_order_relaxed); });

    // 启动多线程狂暴发布
    std::vector<std::thread> publishers;
    for (int i = 0; i < 4; ++i)
    {
        publishers.emplace_back([&]
                                {
            while (running.load(std::memory_order_relaxed)) {
                bus.Publish(ConcurrentEvent{1});
                totalPublished.fetch_add(1, std::memory_order_relaxed);
            } });
    }

    // 模拟频繁上/下线的突变线程
    std::thread mutator([&]
                        {
        while (running.load(std::memory_order_relaxed)) {
            auto token = bus.Subscribe<ConcurrentEvent>([](const ConcurrentEvent&) {});
            std::this_thread::yield();
            bus.Unsubscribe(token);
        } });

    std::this_thread::sleep_for(std::chrono::milliseconds(100));
    running.store(false);

    mutator.join();
    for (auto &t : publishers)
    {
        t.join();
    }

    bus.Unsubscribe(permanentToken);

    // 终极一致性断言：在高频多线程 COW 拓扑更替的血肉磨坊里，常驻节点收到的消息和发出的消息完美相等
    ASSERT_GT(totalPublished.load(), 0);
    EXPECT_EQ(permanentConsumed.load(), totalPublished.load())
        << "Concurrency Race condition caused Data Loss or Duplication!";
}