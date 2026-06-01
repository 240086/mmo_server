// /home/xj/projects/mmo_server/server_core/tests/infrastructure/lockfree_queue/ConsumerStarvationTests.cpp
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
    constexpr int kProducerCount = 8; // 8核高压对撞

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
                        // 队列暴满，出让微小时间片，防止物理总线被锁死，让消费者有喘息之机
                        std::this_thread::sleep_for(std::chrono::microseconds(1));
                        fail_count = 0;
                    }
                }
            } });
    }

    // 2. 初始化采样器与仪表盘
    auto start_time = std::chrono::steady_clock::now();
    auto last_report_time = start_time;
    constexpr auto kDuration = std::chrono::seconds(30); // 压测持续 30 秒

    uint64_t last_consumed_count = 0;
    int seconds_elapsed = 0;

    std::cout << "\n[==================== STARTING 10s ANTI-STARVATION TEST ====================]\n";
    std::cout << std::setw(6) << "Time" << std::setw(18) << "Total Consumed"
              << std::setw(20) << "Interval Consumed" << std::setw(20) << "Starvation Check\n";
    std::cout << "----------------------------------------------------------------------------\n";

    // 3. 消费者主循环
    while (std::chrono::steady_clock::now() - start_time < kDuration)
    {
        uint64_t value = 0;
        if (queue->Dequeue(value))
        {
            // 单线程消费，用宽松递增即可
            consumed.fetch_add(1, std::memory_order_relaxed);
        }

        // 秒级分频断言与心跳
        auto now = std::chrono::steady_clock::now();
        auto interval = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_report_time).count();

        if (interval >= 1000) // 每秒进行一次饥饿审计
        {
            seconds_elapsed++;
            uint64_t current_total_consumed = consumed.load(std::memory_order_relaxed);
            uint64_t interval_consumed = current_total_consumed - last_consumed_count;

            // 🎯 核心架构师设计：硬核防饥饿断言！
            // 这一秒内消费的数据量必须大于 0！如果是 0，说明消费者在过去的 1000ms 内被彻底饿死了！
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
    // 彻底清空残渣
    while (queue->Dequeue(value))
    {
        consumed.fetch_add(1, std::memory_order_relaxed);
    }

    std::cout << "[*] Final Audit -> Global Total Consumed: " << consumed.load() << "\n";
    std::cout << "[==========================================================================]\n\n";

    // 终极存活断言
    ASSERT_GT(consumed.load(), 0u);
}