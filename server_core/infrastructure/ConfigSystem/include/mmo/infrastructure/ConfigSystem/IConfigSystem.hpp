// /home/xj/projects/mmo_server/server_core/infrastructure/ConfigSystem/include/mmo/infrastructure/ConfigSystem/IConfigSystem.hpp
#pragma once

#include <filesystem>
#include <vector>

#include <mmo/infrastructure/ConfigSystem/ConfigTypes.hpp>
#include <mmo/infrastructure/ConfigSystem/ConfigValue.hpp>

namespace mmo::infrastructure::config
{
    class IConfigSystem
    {
    public:
        virtual ~IConfigSystem() = default;

        // 统一加载入口（单文件/目录/多文件）
        virtual bool Load(
            const std::vector<std::filesystem::path> &paths) = 0;

        virtual void Clear() = 0;

        virtual bool IsLoaded() const noexcept = 0;
    };
}