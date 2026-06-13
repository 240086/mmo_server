// /home/xj/projects/mmo_server/server_core/infrastructure/ConfigSystem/include/mmo/infrastructure/ConfigSystem/ConfigTypes.hpp
#pragma once

#include <cstdint>
#include <string>

namespace mmo::infrastructure::config
{
    using ConfigKey = std::string;

    enum class ConfigValueType
    {
        Boolean,
        Integer,
        Double,
        String
    };
}