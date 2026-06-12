// /home/xj/projects/mmo_server/server_core/tests/infrastructure/logger/LoggerInfrastructureTests.cpp
#include <gtest/gtest.h>
#include <vector>
#include <string>
#include <mutex>
#include <thread>
#include <filesystem>
#include <fstream>

#include <mmo/infrastructure/logger/Logger.hpp>
#include <mmo/infrastructure/logger/TextFormatter.hpp>
#include <mmo/infrastructure/logger/FileSink.hpp>
#include <mmo/infrastructure/logger/LoggerConstants.hpp>

using namespace mmo::infrastructure::logger;

// ==========================================
// 线程安全的验证用 MockSink
// ==========================================
class ThreadSafeMockSink : public ILogSink
{
public:
    void Write(std::string_view formattedMessage) override
    {
        std::lock_guard<std::mutex> lock(mutex_);
        captured_logs.emplace_back(std::string(formattedMessage));
    }

    void Flush() override
    {
        flush_called = true;
    }

    std::vector<std::string> captured_logs;
    std::mutex mutex_;
    bool flush_called{false};
};

// ==========================================
// 1. Unit Tests (单元测试组)
// ==========================================

// 验证基本打印与层级过滤
TEST(LoggerUnitErrTest, LevelFilteringVerification)
{
    Logger logger(std::make_unique<TextFormatter>());
    auto mock_sink = std::make_unique<ThreadSafeMockSink>();
    auto *raw_sink = mock_sink.get();

    logger.AddSink(std::move(mock_sink));
    logger.SetMinimumLevel(LogLevel::Info);

    // 低于 Info 级别的日志应被过滤
    logger.Log(LogLevel::Debug, "Filtered Msg");
    EXPECT_TRUE(raw_sink->captured_logs.empty());

    // 等于或高于 Info 级别的日志必须放行
    logger.Log(LogLevel::Info, "Valid Engine Log");
    ASSERT_EQ(raw_sink->captured_logs.size(), 1);
    EXPECT_NE(raw_sink->captured_logs[0].find("Valid Engine Log"), std::string::npos);
}

// 核心确定性测试 (Formatter Determinism Test)
TEST(TextFormatterTest, DeterministicFormatting)
{
    TextFormatter formatter;
    LogRecord record{
        1718167559000, // 固定高精度时间戳 (2024-06-12 12:45:59.000 逻辑模拟)
        9999,          // 固定线程 ID
        LogLevel::Warn,
        "Deterministic Payload Verification"};

    char buf_first[MAX_LOG_FORMAT_BUFFER_SIZE];
    std::string_view first_output = formatter.Format(record, buf_first, sizeof(buf_first));
    std::string first_str(first_output);

    // 连续执行 1000 次格式化，验证输出 100% 具备确定性稳定契约
    for (int i = 0; i < 1000; ++i)
    {
        char buf_loop[MAX_LOG_FORMAT_BUFFER_SIZE];
        std::string_view loop_output = formatter.Format(record, buf_loop, sizeof(buf_loop));
        EXPECT_EQ(first_str, std::string(loop_output));
    }
}

// 多 Sink 广播分发测试 (MultiSink Dispatch Test)
TEST(LoggerUnitErrTest, MultiSinkDispatchVerification)
{
    Logger logger(std::make_unique<TextFormatter>());

    auto sinkA = std::make_unique<ThreadSafeMockSink>();
    auto sinkB = std::make_unique<ThreadSafeMockSink>();
    auto sinkC = std::make_unique<ThreadSafeMockSink>();

    auto *rawA = sinkA.get();
    auto *rawB = sinkB.get();
    auto *rawC = sinkC.get();

    // Phase 0 注册多个 Sink
    logger.AddSink(std::move(sinkA));
    logger.AddSink(std::move(sinkB));
    logger.AddSink(std::move(sinkC));

    // 执行分发
    logger.Log(LogLevel::Error, "Broadcast Test");

    // 验证每个独立的 Sink 都安全完整地收到了该条日志
    ASSERT_EQ(rawA->captured_logs.size(), 1);
    ASSERT_EQ(rawB->captured_logs.size(), 1);
    ASSERT_EQ(rawC->captured_logs.size(), 1);

    EXPECT_EQ(rawA->captured_logs[0], rawB->captured_logs[0]);
    EXPECT_EQ(rawB->captured_logs[0], rawC->captured_logs[0]);
}

// ==========================================
// 2. Boundary Tests (边界安全性测试组)
// ==========================================

// 超长载荷截断安全性测试 (修复了断言 Bug)
TEST(LoggerBoundaryTest, LongPayloadTruncationSafety)
{
    Logger logger(std::make_unique<TextFormatter>());
    auto mock_sink = std::make_unique<ThreadSafeMockSink>();
    auto *raw_sink = mock_sink.get();
    logger.AddSink(std::move(mock_sink));

    // 故意制造大幅度超越栈限制的超长载荷
    std::string giant_payload(MAX_LOG_FORMAT_BUFFER_SIZE * 2, 'K');

    ASSERT_NO_THROW(logger.Log(LogLevel::Error, giant_payload));
    ASSERT_EQ(raw_sink->captured_logs.size(), 1);

    // 修复后的正确断言：MockSink 不带 \n，因此其最大容量等于格式化 Buffer 最大容量减去追加的元数据大小
    // 这里直接断言其不超过缓冲区物理上限减一（留给截断边界的安全空间）
    EXPECT_LE(raw_sink->captured_logs[0].size(), MAX_LOG_FORMAT_BUFFER_SIZE - 1);
    EXPECT_FALSE(raw_sink->captured_logs[0].empty());
}

// 空日志体边界防御
TEST(LoggerBoundaryTest, EmptyMessageTest)
{
    Logger logger(std::make_unique<TextFormatter>());
    auto mock_sink = std::make_unique<ThreadSafeMockSink>();
    auto *raw_sink = mock_sink.get();
    logger.AddSink(std::move(mock_sink));

    ASSERT_NO_THROW(logger.Log(LogLevel::Info, ""));
    ASSERT_EQ(raw_sink->captured_logs.size(), 1);
    EXPECT_FALSE(raw_sink->captured_logs[0].empty()); // 即使消息为空，元数据（时间戳、线程ID）也应正常成文
}

// ==========================================
// 3. IO Boundary Tests (操作系统边界与冲刷契约)
// ==========================================

// 验证 FileSink 物理落盘与 Flush 强契约测试
TEST(LoggerIoContractTest, FileSinkFlushContractVerification)
{
    std::filesystem::path sandbox_log = "flush_contract_test.log";
    if (std::filesystem::exists(sandbox_log))
    {
        std::filesystem::remove(sandbox_log);
    }

    {
        Logger logger(std::make_unique<TextFormatter>());
        auto file_sink = std::make_unique<FileSink>(sandbox_log);
        logger.AddSink(std::move(file_sink));

        logger.Log(LogLevel::Info, "Flush Integrity Token Data");

        // 显式触发冲刷契约，迫使 OS 刷新内核 VFS 缓冲区到用户态可见的文件系统
        logger.Flush();

        // 强契约判定：在 Logger 对象析构生命周期完结之前，外部ifstream必须立即可读该行数据
        ASSERT_TRUE(std::filesystem::exists(sandbox_log));
        std::ifstream ifs(sandbox_log);
        ASSERT_TRUE(ifs.is_open());

        std::string line;
        std::getline(ifs, line);
        EXPECT_NE(line.find("Flush Integrity Token Data"), std::string::npos);
    }

    std::filesystem::remove(sandbox_log);
}

// ==========================================
// 4. IO Boundary Tests: 1000次连续写入持久化验证
// ==========================================
TEST(LoggerIoContractTest, FileSinkMultipleWritesDurability)
{
    std::filesystem::path stress_log_path = "durability_multiple_writes.log";
    if (std::filesystem::exists(stress_log_path))
    {
        std::filesystem::remove(stress_log_path);
    }

    constexpr int WRITE_COUNT = 1000;
    {
        Logger logger(std::make_unique<TextFormatter>());
        auto file_sink = std::make_unique<FileSink>(stress_log_path);
        logger.AddSink(std::move(file_sink));

        for (int i = 0; i < WRITE_COUNT; ++i)
        {
            logger.Log(LogLevel::Info, "Durability Token Line");
        }
        logger.Flush();
    }

    // 跨越 OS 边界验证物理行数完整性
    ASSERT_TRUE(std::filesystem::exists(stress_log_path));
    std::ifstream ifs(stress_log_path);
    int line_count = 0;
    std::string dummy;
    while (std::getline(ifs, dummy))
    {
        line_count++;
    }

    // 验证 1000 次写入无任何行数丢失（No Missing Writes）
    EXPECT_EQ(line_count, WRITE_COUNT);
    std::filesystem::remove(stress_log_path);
}

// ==========================================
// 5. Orchestration Tests: Flush 广播契约验证
// ==========================================
TEST(LoggerOrchestrationTest, FlushBroadcastVerification)
{
    Logger logger(std::make_unique<TextFormatter>());

    auto sinkA = std::make_unique<ThreadSafeMockSink>();
    auto sinkB = std::make_unique<ThreadSafeMockSink>();
    auto sinkC = std::make_unique<ThreadSafeMockSink>();

    auto *rawA = sinkA.get();
    auto *rawB = sinkB.get();
    auto *rawC = sinkC.get();

    logger.AddSink(std::move(sinkA));
    logger.AddSink(std::move(sinkB));
    logger.AddSink(std::move(sinkC));

    // 执行配置阶段后的 Flush 广播
    logger.Flush();

    // 验证所有拓扑内的 Sink 均接收到了 Flush 信号
    EXPECT_TRUE(rawA->flush_called);
    EXPECT_TRUE(rawB->flush_called);
    EXPECT_TRUE(rawC->flush_called);
}