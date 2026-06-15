// server_core/infrastructure/ConfigSystem/src/ConfigSystem.cpp
#include <mmo/infrastructure/ConfigSystem/ConfigSystem.hpp>
#include <mmo/infrastructure/ConfigSystem/parser/IConfigParser.hpp>
#include <mmo/infrastructure/ConfigSystem/parser/KeyValueConfigParser.hpp>

namespace mmo::infrastructure::config
{
    ConfigSystem::ConfigSystem(std::unique_ptr<IConfigParser> parser)
        : parser_(std::move(parser))
    {
        // 策略兜底：如果外部没有注入 mock 或特定 parser，默认使用 V1 格式
        if (!parser_)
        {
            parser_ = std::make_unique<KeyValueConfigParser>();
        }
    }

    bool ConfigSystem::Load(const std::vector<std::filesystem::path> &paths)
    {
        try
        {
            std::unique_lock lock(mutex_);
            if (loaded_)
                return false;

            storage_.clear();

            // 🌟 解耦后的流式调用：完全不在乎底层到底是什么 Parser
            for (const auto &path : paths)
            {
                auto result = parser_->ParseFile(path, storage_);
                if (!result.success)
                {
                    storage_.clear(); // 保持强一致性，失败则彻底回滚
                    return false;
                }
            }

            loaded_ = true;
            return true;
        }
        catch (...)
        {
            storage_.clear();
            loaded_ = false;
            return false;
        }
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