// /home/xj/projects/mmo_server/server_core/infrastructure/ConfigSystem/include/mmo/infrastructure/ConfigSystem/parser/JsonConfigParser.hpp
#pragma once

#include <mmo/infrastructure/ConfigSystem/parser/IConfigParser.hpp>

namespace mmo::infrastructure::config
{
    class KeyValueConfigParser final : public IConfigParser
    {
    public:
        ParseResult ParseFile(
            const std::filesystem::path &path,
            std::unordered_map<ConfigKey, ConfigValue> &output) override;
    };
}