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