#include <gtest/gtest.h>
#include <atomic>
#include <chrono>
#include <thread>
#include <vector>
#include <memory>
#include <iostream>
#include <iomanip>

#include <mmo/infrastructure/lockfree_queue/MPSCQueue.hpp>

using namespace mmo::infrastructure::lockfree_queue;

TEST(MPSCQueueStarvationTest, ConsumerNeverStarves)
{
    constexpr std::size_t kCapacity = 32768;
    constexpr int kProducerCount = 8;

    auto queue = std::make_unique<MPSCQueue<uint64_t, kCapacity>>();
    std::atomic<bool> running{true};
    std::atomic<uint64_t> consumed{0};

    std::vector<std::thread> producers;
    producers.reserve(kProducerCount);

    // 1. 孵化8个全速运转的生产者，带工业级两阶段自愈退避
    for (int p = 0; p < kProducerCount; ++p)
    {
        producers.emplace_back([&queue, &running, p]()
                               {
            uint64_t counter = 0;
            uint32_t fail_count = 0;

            while (running.load(std::memory_order_relaxed))
{
    uint64_t value = (static_cast<uint64_t>(p) << 32) | counter++;

    while (!queue->Enqueue(value))
    {
        // 🎯 核心防死锁绝杀：如果主线程已经发布了叫停命令，且队列满了投递失败
        // 生产者必须立即放弃本次投递，直接撕开内层循环退出，否则会导致主线程 join 处卡死！
        if (!running.load(std::memory_order_relaxed))
        {
            break; 
        }

        if (fail_count++ < 128)
        {
#if defined(__x86_64__) || defined(_M_X64)
            __builtin_ia32_pause();
#else
            std::this_thread::yield();
#endif
        }
        else
        {
            std::this_thread::sleep_for(std::chrono::microseconds(1));
            fail_count = 0;
        }
    }
        } });
    }

    // 2. 初始化采样器与仪表盘（统一对齐为 10 秒快速严苛门禁）
    auto start_time = std::chrono::steady_clock::now();
    auto last_report_time = start_time;
    constexpr auto kDuration = std::chrono::seconds(10);

    uint64_t last_consumed_count = 0;
    int seconds_elapsed = 0;
    uint32_t consumer_fail_count = 0;

    std::cout << "\n[==================== STARTING 10s ANTI-STARVATION TEST ====================]\n";
    std::cout << std::setw(6) << "Time" << std::setw(18) << "Total Consumed" << std::setw(20) << "Interval Consumed" << std::setw(20) << "Starvation Check\n";
    std::cout << "----------------------------------------------------------------------------\n";

    // 3. 消费者主循环
    while (std::chrono::steady_clock::now() - start_time < kDuration)
    {
        uint64_t value = 0;
        if (queue->Dequeue(value))
        {
            consumed.fetch_add(1, std::memory_order_relaxed);
            consumer_fail_count = 0; // 成功则重置
        }
        else
        {
            // 💡 修复：防止空转时频繁调用 steady_clock 轰炸总线
            if (consumer_fail_count++ < 256)
            {
#if defined(__x86_64__) || defined(_M_X64)
                __builtin_ia32_pause();
#else
                std::this_thread::yield();
#endif
            }
            else
            {
                std::this_thread::sleep_for(std::chrono::microseconds(1));
                consumer_fail_count = 0;
            }
        }

        // 秒级分频断言与心跳
        auto now = std::chrono::steady_clock::now();
        auto interval = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_report_time).count();

        if (interval >= 1000)
        {
            seconds_elapsed++;
            uint64_t current_total_consumed = consumed.load(std::memory_order_relaxed);
            uint64_t interval_consumed = current_total_consumed - last_consumed_count;

            // 🎯 核心断言：这一秒内消费的数据量必须大于 0！
            EXPECT_GT(interval_consumed, 0u) << "CRITICAL ERROR: Consumer starved at second " << seconds_elapsed;

            std::cout << std::setw(4) << seconds_elapsed << "s"
                      << std::setw(17) << current_total_consumed
                      << std::setw(19) << interval_consumed
                      << std::setw(20) << (interval_consumed > 0 ? "HEALTHY (ALIVE)" : "STARVED") << "\n"
                      << std::flush;

            last_report_time = now;
            last_consumed_count = current_total_consumed;
        }
    }

    std::cout << "----------------------------------------------------------------------------\n";
    std::cout << "[*] 10 Seconds test complete. Stopping 8 fierce producers...\n"
              << std::flush;

    // 4. 安全离场收尾
    running.store(false, std::memory_order_release);

    for (auto &thread : producers)
    {
        if (thread.joinable())
            thread.join();
    }

    uint64_t value = 0;
    while (queue->Dequeue(value))
    {
        consumed.fetch_add(1, std::memory_order_relaxed);
    }

    std::cout << "[*] Final Audit -> Global Total Consumed: " << consumed.load() << "\n";
    std::cout << "[==========================================================================]\n\n";

    ASSERT_GT(consumed.load(), 0u);
}