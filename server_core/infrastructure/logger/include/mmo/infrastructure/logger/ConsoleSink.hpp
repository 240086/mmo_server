// /home/xj/projects/mmo_server/server_core/infrastructure/logger/include/mmo/infrastructure/logger/ConsoleSink.hpp
#pragma once
#include <mmo/infrastructure/logger/ILogSink.hpp>

namespace mmo::infrastructure::logger
{
    class ConsoleSink final : public ILogSink
    {
    public:
        void Write(std::string_view formattedMessage) override;
        void Flush() override;
    };
}