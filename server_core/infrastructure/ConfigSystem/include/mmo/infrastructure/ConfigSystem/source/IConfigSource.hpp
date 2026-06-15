// /home/xj/projects/mmo_server/server_core/infrastructure/ConfigSystem/include/mmo/infrastructure/ConfigSystem/source/IConfigSource.hpp
#pragma once

#include <filesystem>
#include <vector>

namespace mmo::infrastructure::config
{
    class IConfigSource
    {
    public:
        virtual ~IConfigSource() = default;
        virtual std::vector<std::filesystem::path> Collect() = 0;
    };
}