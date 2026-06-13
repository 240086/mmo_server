// /home/xj/projects/mmo_server/server_core/infrastructure/ConfigSystem/include/mmo/infrastructure/ConfigSystem/parser/IConfigParser.hpp
#pragma once

#include <filesystem>
#include <unordered_map>

#include <mmo/infrastructure/ConfigSystem/ConfigTypes.hpp>
#include <mmo/infrastructure/ConfigSystem/ConfigValue.hpp>
#include <mmo/infrastructure/ConfigSystem/parser/ParseResult.hpp>

namespace mmo::infrastructure::config
{
    class IConfigParser
    {
    public:
        virtual ~IConfigParser() = default;

        virtual ParseResult ParseFile(
            const std::filesystem::path &path,
            std::unordered_map<ConfigKey, ConfigValue> &output) = 0;
    };
}