// /home/xj/projects/mmo_server/server_core/infrastructure/logger/include/mmo/infrastructure/logger/Logger.hpp
#pragma once
#include <memory>
#include <vector>
#include <string_view>
#include <mmo/infrastructure/logger/LogLevel.hpp>
#include <mmo/infrastructure/logger/IFormatter.hpp>
#include <mmo/infrastructure/logger/ILogSink.hpp>

namespace mmo::infrastructure::logger
{
    class Logger final
    {
    public:
        explicit Logger(std::unique_ptr<IFormatter> formatter);

        ~Logger() = default;
        Logger(const Logger &) = delete;
        Logger &operator=(const Logger &) = delete;

        void AddSink(std::unique_ptr<ILogSink> sink);
        void SetMinimumLevel(LogLevel level) noexcept;

        [[nodiscard]] LogLevel GetMinimumLevel() const noexcept;

        // 核心热路径入口
        void Log(LogLevel level, std::string_view message);
        void Flush();

    private:
        LogLevel minimumLevel_{LogLevel::Info};
        std::unique_ptr<IFormatter> formatter_;
        std::vector<std::unique_ptr<ILogSink>> sinks_;
    };
}