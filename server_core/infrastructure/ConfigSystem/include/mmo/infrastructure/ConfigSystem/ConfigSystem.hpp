// /home/xj/projects/mmo_server/server_core/infrastructure/ConfigSystem/include/mmo/infrastructure/ConfigSystem/ConfigSystem.hpp
#pragma once

#include <filesystem>
#include <shared_mutex>
#include <unordered_map>

#include <mmo/infrastructure/ConfigSystem/IConfigSystem.hpp>

namespace mmo::infrastructure::config
{
    class ConfigException : public std::runtime_error
    {
        using std::runtime_error::runtime_error;
    };

    class ConfigNotFoundException : public ConfigException
    {
        using ConfigException::ConfigException;
    };

    class ConfigTypeMismatchException : public ConfigException
    {
        using ConfigException::ConfigException;
    };

    class ConfigSystem final : public IConfigSystem
    {
    public:
        ConfigSystem() = default;
        ~ConfigSystem() override = default;

        ConfigSystem(const ConfigSystem &) = delete;
        ConfigSystem &operator=(const ConfigSystem &) = delete;
        ConfigSystem(ConfigSystem &&) = delete;
        ConfigSystem &operator=(ConfigSystem &&) = delete;

    public:
        bool Load(
            const std::vector<std::filesystem::path> &paths) override;

        void Clear() override;

        bool IsLoaded() const noexcept override;

    public:
        template <typename TValue>
        const TValue &Get(const ConfigKey &key) const
        {
            // Load完成后进入“冻结态”：无锁访问（关键优化点）
            auto iter = storage_.find(key);
            if (iter == storage_.end())
            {
                throw ConfigNotFoundException(key);
            }

            if (!std::holds_alternative<TValue>(iter->second))
            {
                throw ConfigTypeMismatchException(key);
            }

            return std::get<TValue>(iter->second);
        }

        template <typename TValue>
        const TValue *TryGet(const ConfigKey &key) const noexcept
        {
            auto iter = storage_.find(key);
            if (iter == storage_.end())
                return nullptr;

            return std::get_if<TValue>(&iter->second);
        }

    private:
        using Storage =
            std::unordered_map<ConfigKey, ConfigValue>;

        Storage storage_;

        bool loaded_{false};

        mutable std::shared_mutex mutex_;
    };
}