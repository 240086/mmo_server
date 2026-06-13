// /home/xj/projects/mmo_server/server_core/infrastructure/ConfigSystem/include/mmo/infrastructure/ConfigSystem/ConfigValue.hpp
#pragma once

#include <string>
#include <variant>
#include <cstdint>

namespace mmo::infrastructure::config
{
    using ConfigValue =
        std::variant<
            bool,
            std::int64_t,
            double,
            std::string>;
}