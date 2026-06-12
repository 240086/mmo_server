// /home/xj/projects/mmo_server/server_core/infrastructure/logger/include/mmo/infrastructure/logger/TextFormatter.hpp
#pragma once
#include <mmo/infrastructure/logger/IFormatter.hpp>

namespace mmo::infrastructure::logger
{
    class TextFormatter final : public IFormatter
    {
    public:
        std::string_view Format(
            const LogRecord &record,
            char *buffer,
            std::size_t bufferSize) override;
    };
}