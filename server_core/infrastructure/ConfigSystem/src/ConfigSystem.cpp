#include <mmo/infrastructure/ConfigSystem/ConfigSystem.hpp>
#include <mmo/infrastructure/ConfigSystem/parser/IConfigParser.hpp>
#include <mmo/infrastructure/ConfigSystem/parser/JsonConfigParser.hpp>

#include <mutex>

namespace mmo::infrastructure::config
{
    bool ConfigSystem::Load(const std::vector<std::filesystem::path> &paths)
    {
        std::unique_lock lock(mutex_);

        if (loaded_)
            return false;

        storage_.clear();

        JsonConfigParser parser;

        for (const auto &path : paths)
        {
            auto result = parser.ParseFile(path, storage_);

            if (!result.success)
            {
                return false; // fail fast
            }
        }

        loaded_ = true;
        return true;
    }

    void ConfigSystem::Clear()
    {
        std::unique_lock lock(mutex_);
        storage_.clear();
        loaded_ = false;
    }

    bool ConfigSystem::IsLoaded() const noexcept
    {
        std::shared_lock lock(mutex_);
        return loaded_;
    }
}