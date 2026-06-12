// /home/xj/projects/mmo_server/server_core/infrastructure/logger/include/mmo/infrastructure/logger/LogLevel.hpp
#pragma once
#include <cstdint>

namespace mmo::infrastructure::logger
{
    enum class LogLevel : std::uint8_t
    {
        Trace = 0,
        Debug = 1,
        Info = 2,
        Warn = 3,
        Error = 4,
        Fatal = 5
    };
}