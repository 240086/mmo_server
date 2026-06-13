// /home/xj/projects/mmo_server/server_core/tests/infrastructure/EventBus/EventBusBasicTests.cpp
#include <gtest/gtest.h>
#include <chrono>
#include <thread>
#include <vector>
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

// ============================================================================
// EventBus 基础功能测试基线 (Basic Functional Tests)
// ============================================================================

TEST(EventBusBasicTests, SingleSubscriber)
{
    EventBus bus;
    int result = 0;

    bus.Subscribe<TestEvent>(
        [&](const TestEvent &e)
        {
            result = e.value;
        });

    bus.Publish(TestEvent{42});
    EXPECT_EQ(result, 42);
}

TEST(EventBusBasicTests, MultipleSubscribers)
{
    EventBus bus;
    int count = 0;

    bus.Subscribe<TestEvent>([&](const TestEvent &)
                             { ++count; });
    bus.Subscribe<TestEvent>([&](const TestEvent &)
                             { ++count; });

    bus.Publish(TestEvent{});
    EXPECT_EQ(count, 2);
}

TEST(EventBusBasicTests, Unsubscribe)
{
    EventBus bus;
    int count = 0;

    auto token = bus.Subscribe<TestEvent>([&](const TestEvent &)
                                          { ++count; });

    bus.Publish(TestEvent{});
    bus.Unsubscribe(token);
    bus.Publish(TestEvent{});

    EXPECT_EQ(count, 1);
}

TEST(EventBusBasicTests, EmptyPublish)
{
    EventBus bus;
    EXPECT_NO_THROW(bus.Publish(TestEvent{}));
}

TEST(EventBusBasicTests, DifferentEventTypes)
{
    struct EventA
    {
        int value;
    };
    struct EventB
    {
        int value;
    };

    EventBus bus;
    int a = 0;
    int b = 0;

    bus.Subscribe<EventA>([&](const EventA &e)
                          { a = e.value; });
    bus.Subscribe<EventB>([&](const EventB &e)
                          { b = e.value; });

    bus.Publish(EventA{10});
    bus.Publish(EventB{20});

    EXPECT_EQ(a, 10);
    EXPECT_EQ(b, 20);
}

// ============================================================================
// EventBus 高阶多线程与确定性测试 (Advanced Mutex & Correctness Tests)
// ============================================================================

TEST(EventBusAdvancedTests, SelfUnsubscribeInsideCallback)
{
    EventBus bus;
    std::atomic<int> triggerCount{0};
    SubscriptionToken token;

    // 订阅一个在触发时会“自杀”的监听器
    token = bus.Subscribe<TestEvent>([&](const TestEvent &e)
                                     {
        triggerCount++;
        // 核心重构死角验证：在遍历热路径中执行解绑
        bus.Unsubscribe(token); });

    // 额外注册一个常规监听器，紧跟其后，用于验证迭代器没有踩空失效
    bus.Subscribe<TestEvent>([&](const TestEvent &)
                             { triggerCount++; });

    // 第一次发布：应该两个都触发
    bus.Publish(TestEvent{100});
    EXPECT_EQ(triggerCount.load(), 2);

    // 第二次发布：自杀的那个不应该再触发，只有第二个触发
    bus.Publish(TestEvent{200});
    EXPECT_EQ(triggerCount.load(), 3);
}

TEST(EventBusAdvancedTests, ConcurrentMutationAndPublishStorm)
{
    EventBus bus;
    std::atomic<bool> running{true};

    // 正确性指标：精确统计总发布量与稳定消费量
    std::atomic<uint64_t> totalPublished{0};
    std::atomic<uint64_t> permanentTriggered{0};

    // 1. 在风暴前注入常驻监听器（逻辑不变性基石）
    auto permanentToken = bus.Subscribe<TestEvent>([&](const TestEvent &)
                                                   { permanentTriggered.fetch_add(1, std::memory_order_relaxed); });

    // 2. 启动 4 个狂暴发布线程群
    std::vector<std::thread> publishers;
    for (int i = 0; i < 4; ++i)
    {
        publishers.emplace_back([&]()
                                {
            while (running.load(std::memory_order_relaxed)) {
                bus.Publish(TestEvent{42});
                totalPublished.fetch_add(1, std::memory_order_relaxed);
                
                // 动态引入全新未注册类型，强制倒逼外层 Map 在运行时动态 Rehash 扩容！
                struct DynamicallyCreatedEvent { char padding[16]; };
                bus.Publish(DynamicallyCreatedEvent{});
            } });
    }

    // 3. 启动动态订阅/退订线程（频繁制造拓扑突变与墓碑）
    std::thread mutator([&]()
                        {
        uint64_t loop = 0;
        while (running.load(std::memory_order_relaxed)) {
            auto token = bus.Subscribe<TestEvent>([](const TestEvent&) {});
            if (loop % 2 == 0) {
                bus.Unsubscribe(token); 
            }
            std::this_thread::yield();
            loop++;
        } });

    // 持续高压冲击 50 毫秒
    std::this_thread::sleep_for(std::chrono::milliseconds(50));
    running.store(false);

    // 回收所有并发线程
    mutator.join();
    for (auto &t : publishers)
    {
        t.join();
    }

    // 4. 清理常驻标记
    bus.Unsubscribe(permanentToken);

    // 5. 核心正确性硬核断言
    uint64_t finalPublished = totalPublished.load();
    uint64_t finalTriggered = permanentTriggered.load();

    ASSERT_GT(finalPublished, 0) << "Test infrastructure error: No events were published.";

    // 终极断言：在经历了上万次快照 COW 彻底更替和 Rehash 后，没有任何一个发布出来的事件被漏掉或重复投递！
    EXPECT_EQ(finalTriggered, finalPublished)
        << "Data Loss or Duplication detected under concurrent mutation! "
        << "Published: " << finalPublished << ", Received: " << finalTriggered;
}

TEST(EventBusAdvancedTests, EventStormThroughputBenchmark)
{
    EventBus bus;
    constexpr int ITERATIONS = 1000000;
    std::atomic<uint64_t> executed{0};

    // 捕获一部分上下文，刻意撑大 Lambda 尺寸
    std::string contextBait = "infrastructure_event_bus_benchmark_context_heavy_string";
    bus.Subscribe<TestEvent>([&executed, contextBait](const TestEvent &e)
                             { executed.fetch_add(e.value, std::memory_order_relaxed); });

    auto start = std::chrono::high_resolution_clock::now();

    // 执行百万级风暴冲击
    for (int i = 0; i < ITERATIONS; ++i)
    {
        bus.Publish(TestEvent{1});
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> elapsed = end - start;

    // 彻底去除了破坏 CI 稳定性的 EXPECT_LT 绝对时间断言
    EXPECT_EQ(executed.load(), ITERATIONS);

    std::cout << "[ BENCHMARK ] 1M Events Dispatched in: " << elapsed.count() << " ms" << std::endl;
}