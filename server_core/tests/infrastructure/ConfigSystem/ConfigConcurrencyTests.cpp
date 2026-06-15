// /home/xj/projects/mmo_server/server_core/tests/infrastructure/ConfigSystem/ConfigConcurrencyTests.cpp
#include <gtest/gtest.h>

#include <atomic>
#include <fstream>
#include <thread>

#include <mmo/infrastructure/ConfigSystem/ConfigSystem.hpp>

using namespace mmo::infrastructure::config;

TEST(ConfigConcurrencyTests, MultiThreadReadStorm)
{
    auto path =
        std::filesystem::temp_directory_path() /
        "storm.cfg";

    {
        std::ofstream f(path);

        f << "name=test\n";
    }

    ConfigSystem config;

    ASSERT_TRUE(config.Load({path}));

    std::atomic<bool> failed{false};

    std::vector<std::thread> workers;

    for (int i = 0; i < 8; ++i)
    {
        workers.emplace_back(
            [&]()
            {
                for (int j = 0;
                     j < 100000;
                     ++j)
                {
                    auto const &value =
                        config.Get<std::string>(
                            "name");

                    if (value != "test")
                    {
                        failed = true;
                    }
                }
            });
    }

    for (auto &t : workers)
    {
        t.join();
    }

    EXPECT_FALSE(
        failed.load());
}