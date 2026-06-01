// /home/xj/projects/mmo_server/server_core/tests/infrastructure/lockfree_queue/LongDurationStabilityTests.cpp
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

TEST(MPSCQueueRuntimeTest, LongDurationStability)
{
    constexpr std::size_t kCapacity = 65536;
    constexpr int kProducerCount = 4;

    // 1. 物理隔离：堆分配，杜绝巨无霸对象撑爆栈空间
    auto queue = std::make_unique<MPSCQueue<uint64_t, kCapacity>>();

    std::atomic<bool> running{true};

    // 生产者局部计数阵列，彻底绝育多核高频 fetch_add 产生的总线风暴
    std::vector<uint64_t> producer_counts(kProducerCount, 0);

    std::atomic<uint64_t> consumed{0};
    std::vector<std::thread> producers;
    producers.reserve(kProducerCount);

    for (int p = 0; p < kProducerCount; ++p)
    {
        producers.emplace_back([&queue, &running, &producer_counts, p]()
                               {
            uint64_t local = 0;
            uint32_t fail_count = 0; // 工业级智能退避计数器

            while (running.load(std::memory_order_relaxed))
            {
                uint64_t value = (static_cast<uint64_t>(p) << 48) | local;

                if (queue->Enqueue(value))
                {
                    ++local;
                    fail_count = 0; // 成功后立刻重置退避
                }
                else
                {
                    // 💡 生产者两阶段退避：防止队列满时疯狂自旋冲垮虚拟化调度
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
                        // 严重堵塞，出让微小时间片，给消费者腾出 CPU 核心
                        std::this_thread::sleep_for(std::chrono::microseconds(1));
                        fail_count = 0;
                    }
                }
            }
            producer_counts[p] = local; });
    }

    // 2. 准备时间锚点与实时回显仪表盘
    auto start_time = std::chrono::steady_clock::now();
    auto last_report_time = start_time;
    constexpr auto kTestDuration = std::chrono::seconds(30);

    uint64_t last_consumed_count = 0;
    int seconds_elapsed = 0;
    uint32_t consumer_fail_count = 0; // 消费者智能退避计数器

    std::cout << "\n[==================== STARTING 30s STABILITY TEST ====================]\n";
    std::cout << std::setw(6) << "Time" << std::setw(18) << "Total Consumed"
              << std::setw(20) << "Current Interval" << std::setw(20) << "Real-time Speed\n";
    std::cout << "------------------------------------------------------------------------\n";

    // 3. 消费者主循环 + 秒级分频心跳
    while (std::chrono::steady_clock::now() - start_time < kTestDuration)
    {
        uint64_t value = 0;
        if (queue->Dequeue(value))
        {
            consumed.fetch_add(1, std::memory_order_relaxed);
            consumer_fail_count = 0; // 成功后立刻重置退避
        }
        else
        {
            // 💡 消费者两阶段退避：防止空转时吃满单核，对抗 WSL2 的 vCPU 窃取
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
                // 队列暂时真空，主动休眠 1 微秒，抚平潜在的总线饥饿
                std::this_thread::sleep_for(std::chrono::microseconds(1));
                consumer_fail_count = 0;
            }
        }

        // 秒级分频采样器
        auto now = std::chrono::steady_clock::now();
        auto interval = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_report_time).count();

        if (interval >= 1000)
        {
            seconds_elapsed++;
            uint64_t current_total_consumed = consumed.load(std::memory_order_relaxed);
            uint64_t interval_consumed = current_total_consumed - last_consumed_count;

            double sampled_ops = static_cast<double>(interval_consumed);
            double sampled_time = static_cast<double>(interval);

            double speed = (sampled_ops / 1000000.0) / (sampled_time / 1000.0);

            std::cout << std::setw(4) << seconds_elapsed << "s"
                      << std::setw(17) << current_total_consumed
                      << std::setw(19) << interval_consumed
                      << std::setw(14) << std::fixed << std::setprecision(2) << speed << " M ops/sec\n"
                      << std::flush;

            last_report_time = now;
            last_consumed_count = current_total_consumed;
        }
    }

    std::cout << "------------------------------------------------------------------------\n";
    std::cout << "[*] 30 Seconds time up! Stopping all producers...\n"
              << std::flush;

    // 4. 发令枪停产并安全收尾
    running.store(false, std::memory_order_release);

    for (auto &thread : producers)
    {
        if (thread.joinable())
            thread.join();
    }

    std::cout << "[*] All producers stopped. Draining remaining queue pipeline...\n"
              << std::flush;

    uint64_t value = 0;
    // 清空收尾残渣
    while (queue->Dequeue(value))
    {
        consumed.fetch_add(1, std::memory_order_relaxed);
    }

    // 汇总生产总量
    uint64_t total_produced = 0;
    for (int p = 0; p < kProducerCount; ++p)
    {
        total_produced += producer_counts[p];
    }

    std::cout << "[*] Audit Report -> Total Produced: " << total_produced
              << " | Total Consumed: " << consumed.load() << "\n";
    std::cout << "[======================================================================]\n\n";

    // 终极对齐断言
    ASSERT_EQ(total_produced, consumed.load());
}