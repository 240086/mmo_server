// /home/xj/projects/mmo_server/server_core/infrastructure/logger/include/mmo/infrastructure/logger/FileSink.hpp
#pragma once
#include <filesystem>
#include <mmo/infrastructure/logger/ILogSink.hpp>

namespace mmo::infrastructure::logger
{
    class FileSink final : public ILogSink
    {
    public:
        explicit FileSink(const std::filesystem::path &path);
        ~FileSink() override;

        // 禁用拷贝与移动，强制独占资源契约
        FileSink(const FileSink &) = delete;
        FileSink &operator=(const FileSink &) = delete;
        FileSink(FileSink &&) noexcept = delete;
        FileSink &operator=(FileSink &&) noexcept = delete;

        void Write(std::string_view formattedMessage) override;
        void Flush() override;

    private:
        class Impl;
        Impl *impl_; // PImpl 模式，完美阻断 <fstream> 带来的编译期污染
    };
}