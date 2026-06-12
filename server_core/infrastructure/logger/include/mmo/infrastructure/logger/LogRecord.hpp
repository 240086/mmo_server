// /home/xj/projects/mmo_server/server_core/infrastructure/logger/include/mmo/infrastructure/logger/LogRecord.hpp
#pragma once
#include <string_view>
#include <mmo/infrastructure/logger/LoggerTypes.hpp>
#include <mmo/infrastructure/logger/LogLevel.hpp>

namespace mmo::infrastructure::logger
{
    struct LogRecord
    {
        Timestamp timestamp{};
        ThreadId threadId{};
        LogLevel level{LogLevel::Info};
        std::string_view message{};
    };
}