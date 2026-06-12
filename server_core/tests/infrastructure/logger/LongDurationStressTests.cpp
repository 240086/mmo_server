// /home/xj/projects/mmo_server/server_core/tests/infrastructure/logger/LongDurationStressTests.cpp
#include <gtest/gtest.h>
#include <thread>
#include <vector>
#include <atomic>
#include <mmo/infrastructure/logger/Logger.hpp>
#include <mmo/infrastructure/logger/TextFormatter.hpp>

using namespace mmo::infrastructure::logger;

// 专门针对工业高吞吐压力订制的零堆分配计数 Sink
class AtomicCounterSink : public ILogSink
{
public:
    void Write(std::string_view formattedMessage) override
    {
        if (!formattedMessage.empty())
        {
            // 仅执行原子递增，确保压力测试纯粹聚焦于前端并发的 Orchestration + Formatter 栈分配分配安全性
            total_log_count.fetch_add(1, std::memory_order_relaxed);
        }
    }

    void Flush() override {}

    std::atomic<uint64_t> total_log_count{0};
};

// 16线程 * 100,000次 = 1,600,000 条极大规模高频日志爆发冲刷压力测试
TEST(LoggerHighDurationStressTest, MultiThreadedBurstPressure)
{
    Logger logger(std::make_unique<TextFormatter>());
    auto counter_sink = std::make_unique<AtomicCounterSink>();
    auto *raw_sink = counter_sink.get();

    logger.AddSink(std::move(counter_sink));
    logger.SetMinimumLevel(LogLevel::Trace);

    constexpr int CONCURRENT_THREADS = 16;
    constexpr int BURST_PER_THREAD = 100000;

    std::vector<std::thread> thread_pool;
    thread_pool.reserve(CONCURRENT_THREADS);

    // 16个前台线程无锁疯狂挤压并发冲击 Logger::Log
    for (int i = 0; i < CONCURRENT_THREADS; ++i)
    {
        thread_pool.emplace_back([&logger]()
                                 {
            for (int j = 0; j < BURST_PER_THREAD; ++j)
            {
                logger.Log(LogLevel::Debug, "Industrial scale stress logging trace payload text");
            } });
    }

    // 等待所有压力线程冲刷完毕
    for (auto &worker : thread_pool)
    {
        worker.join();
    }

    // 终极断言：验证在高强度的无锁哈希、栈并发转换和原子合并下，数据不发生任何丢失
    EXPECT_EQ(raw_sink->total_log_count.load(), static_cast<uint64_t>(CONCURRENT_THREADS * BURST_PER_THREAD));
}