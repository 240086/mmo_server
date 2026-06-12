#include <chrono>
#include <thread>
#include <functional>
#include <mmo/infrastructure/logger/Logger.hpp>
#include <mmo/infrastructure/logger/LoggerConstants.hpp>

namespace mmo::infrastructure::logger
{

    namespace
    {
        // 升级为毫秒精度
        inline Timestamp NowMilliseconds()
        {
            using namespace std::chrono;
            return static_cast<Timestamp>(
                duration_cast<milliseconds>(
                    system_clock::now().time_since_epoch())
                    .count());
        }

        // 利用 thread_local 消除热路径下的 std::hash 性能损耗
        inline ThreadId GetCachedThreadId()
        {
            thread_local static const ThreadId cached_id = static_cast<ThreadId>(
                std::hash<std::thread::id>{}(std::this_thread::get_id()));
            return cached_id;
        }
    }

    Logger::Logger(std::unique_ptr<IFormatter> formatter)
        : formatter_(std::move(formatter))
    {
    }

    void Logger::AddSink(std::unique_ptr<ILogSink> sink)
    {
        sinks_.emplace_back(std::move(sink));
    }

    void Logger::SetMinimumLevel(LogLevel level) noexcept
    {
        minimumLevel_ = level;
    }

    LogLevel Logger::GetMinimumLevel() const noexcept
    {
        return minimumLevel_;
    }

    void Logger::Log(LogLevel level, std::string_view message)
    {
        if (level < minimumLevel_)
        {
            return;
        }

        // 确定性的前台线程栈分配，100% 避免堆分配
        char buffer[MAX_LOG_FORMAT_BUFFER_SIZE];

        LogRecord record{
            NowMilliseconds(),
            GetCachedThreadId(),
            level,
            message};

        auto formatted = formatter_->Format(
            record,
            buffer,
            sizeof(buffer));

        // O(N) 严格分发
        for (auto &sink : sinks_)
        {
            sink->Write(formatted);
        }
    }

    void Logger::Flush()
    {
        for (auto &sink : sinks_)
        {
            sink->Flush();
        }
    }

}