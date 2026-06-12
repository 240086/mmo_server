#include <iostream>
#include <mutex>

#include <mmo/infrastructure/logger/ConsoleSink.hpp>

namespace mmo::infrastructure::logger
{

    namespace
    {
        std::mutex g_console_mutex;
    }

    void ConsoleSink::Write(std::string_view formattedMessage)
    {
        std::lock_guard<std::mutex> lock(g_console_mutex);

        std::cout.write(
            formattedMessage.data(),
            static_cast<std::streamsize>(formattedMessage.size()));

        std::cout.put('\n');
    }

    void ConsoleSink::Flush()
    {
        std::lock_guard<std::mutex> lock(g_console_mutex);
        std::cout.flush();
    }

}