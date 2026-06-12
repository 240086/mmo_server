#include <fstream>
#include <mutex>

#include <mmo/infrastructure/logger/FileSink.hpp>

namespace mmo::infrastructure::logger
{

    class FileSink::Impl
    {
    public:
        std::ofstream file;
        std::mutex mutex;
    };

    FileSink::FileSink(const std::filesystem::path &path)
        : impl_(new Impl())
    {
        impl_->file.open(path, std::ios::out | std::ios::app);
    }

    FileSink::~FileSink()
    {
        Flush();
        delete impl_;
    }

    void FileSink::Write(std::string_view formattedMessage)
    {
        std::lock_guard<std::mutex> lock(impl_->mutex);

        impl_->file.write(
            formattedMessage.data(),
            static_cast<std::streamsize>(formattedMessage.size()));

        impl_->file.put('\n');
    }

    void FileSink::Flush()
    {
        std::lock_guard<std::mutex> lock(impl_->mutex);
        impl_->file.flush();
    }

}