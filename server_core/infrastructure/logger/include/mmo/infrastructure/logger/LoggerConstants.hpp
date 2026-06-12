// /home/xj/projects/mmo_server/server_core/infrastructure/logger/include/mmo/infrastructure/logger/LoggerConstants.hpp
#pragma once
#include <cstdint>

namespace mmo::infrastructure::logger
{
    // 限制单条日志格式化后的最大长度，防止栈溢出，同时契约化前台栈分配大小
    inline constexpr std::size_t MAX_LOG_FORMAT_BUFFER_SIZE = 2048;
}