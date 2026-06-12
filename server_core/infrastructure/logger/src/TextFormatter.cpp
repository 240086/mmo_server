#include <cstdio>
#include <cstring>
#include <ctime>
#include <mmo/infrastructure/logger/TextFormatter.hpp>
#include <mmo/infrastructure/logger/LoggerConstants.hpp>

namespace mmo::infrastructure::logger
{

    std::string_view TextFormatter::Format(
        const LogRecord &record,
        char *buffer,
        std::size_t bufferSize)
    {
        if (!buffer || bufferSize == 0)
        {
            return {};
        }

        // 提取出秒数用于格式化日期，保留毫秒余数
        std::uint64_t total_ms = record.timestamp;
        std::time_t t = static_cast<std::time_t>(total_ms / 1000);
        std::uint32_t ms = static_cast<std::uint32_t>(total_ms % 1000);

        std::tm tm{};
#if defined(_WIN32)
        localtime_s(&tm, &t);
#else
        localtime_r(&t, &tm);
#endif

        // 增加 .%03d 毫秒级别高精度打印
        int written = std::snprintf(
            buffer,
            bufferSize,
            "%04d-%02d-%02d %02d:%02d:%02d.%03d [T%u] [%d] %.*s",
            tm.tm_year + 1900,
            tm.tm_mon + 1,
            tm.tm_mday,
            tm.tm_hour,
            tm.tm_min,
            tm.tm_sec,
            ms,
            record.threadId,
            static_cast<int>(record.level),
            static_cast<int>(record.message.size()),
            record.message.data());

        if (written < 0)
        {
            return {};
        }

        // 严格边界防御：若被截断，返回不含底层自动追加 '\0' 的最大可用长度
        if (static_cast<std::size_t>(written) >= bufferSize)
        {
            return std::string_view(buffer, bufferSize - 1);
        }

        return std::string_view(buffer, static_cast<std::size_t>(written));
    }

}