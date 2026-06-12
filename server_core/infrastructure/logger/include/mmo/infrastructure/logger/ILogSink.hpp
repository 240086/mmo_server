// /home/xj/projects/mmo_server/server_core/infrastructure/logger/include/mmo/infrastructure/logger/ILogSink.hpp
#pragma once
#include <string_view>

namespace mmo::infrastructure::logger
{
    class ILogSink
    {
    public:
        virtual ~ILogSink() = default;

        // 只接受格式化完成后的只读视图，职责单一化
        virtual void Write(std::string_view formattedMessage) = 0;
        virtual void Flush() = 0;
    };
}