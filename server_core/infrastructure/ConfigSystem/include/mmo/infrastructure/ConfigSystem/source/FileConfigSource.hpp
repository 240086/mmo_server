// /home/xj/projects/mmo_server/server_core/infrastructure/ConfigSystem/include/mmo/infrastructure/ConfigSystem/source/FileConfigSource.hpp
#pragma once

#include <filesystem>
#include <vector>

#include <mmo/infrastructure/ConfigSystem/source/IConfigSource.hpp>

namespace mmo::infrastructure::config
{
    class FileConfigSource final : public IConfigSource
    {
    public:
        explicit FileConfigSource(std::filesystem::path root);

        std::vector<std::filesystem::path> Collect() override;

    private:
        std::filesystem::path root_;
    };
}