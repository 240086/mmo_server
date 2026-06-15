#include <mmo/infrastructure/ConfigSystem/source/FileConfigSource.hpp>

namespace mmo::infrastructure::config
{
    FileConfigSource::FileConfigSource(
        std::filesystem::path root)
        : root_(std::move(root))
    {
    }

    std::vector<std::filesystem::path>
    FileConfigSource::Collect()
    {
        std::vector<std::filesystem::path> result;

        if (std::filesystem::is_regular_file(root_))
        {
            result.push_back(root_);
            return result;
        }

        if (!std::filesystem::exists(root_))
        {
            return result;
        }

        for (auto const &entry :
             std::filesystem::recursive_directory_iterator(root_))
        {
            if (!entry.is_regular_file())
            {
                continue;
            }

            result.push_back(entry.path());
        }

        return result;
    }
}