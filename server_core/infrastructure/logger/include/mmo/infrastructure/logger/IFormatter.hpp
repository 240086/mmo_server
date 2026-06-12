// /home/xj/projects/mmo_server/server_core/infrastructure/logger/include/mmo/infrastructure/logger/IFormatter.hpp
#pragma once
#include <string_view>
#include <cstddef>
#include <mmo/infrastructure/logger/LogRecord.hpp>

namespace mmo::infrastructure::logger
{
    class IFormatter
    {
    public:
        virtual ~IFormatter() = default;

        // 强约束：严禁内部申请堆内存，必须就地利用 caller 提供的 buffer 填充并返回视图
        virtual std::string_view Format(
            const LogRecord &record,
            char *buffer,
            std::size_t bufferSize) = 0;
    };
}