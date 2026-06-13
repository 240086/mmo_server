// /home/xj/projects/mmo_server/server_core/infrastructure/ConfigSystem/include/mmo/infrastructure/ConfigSystem/parser/ParseResult.hpp
#pragma once

#include <string>
#include <vector>

namespace mmo::infrastructure::config
{
    struct ParseError
    {
        std::string message;
        std::string file;
        int line{0};
    };

    struct ParseResult
    {
        bool success{false};
        std::vector<ParseError> errors;
    };
}