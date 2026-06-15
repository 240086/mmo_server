// /home/xj/projects/mmo_server/server_core/tests/infrastructure/ConfigSystem/ConfigLoadTests.cpp
#include <gtest/gtest.h>

#include <fstream>

#include <mmo/infrastructure/ConfigSystem/ConfigSystem.hpp>

using namespace mmo::infrastructure::config;

namespace
{
    std::filesystem::path CreateConfig(
        const std::string &name,
        const std::string &content)
    {
        auto path =
            std::filesystem::temp_directory_path() / name;

        std::ofstream file(path);
        file << content;

        return path;
    }
}

TEST(ConfigLoadTests, SingleFileLoad)
{
    auto path =
        CreateConfig(
            "config_single.cfg",
            "server_name=test\n");

    ConfigSystem config;

    EXPECT_TRUE(config.Load({path}));
    EXPECT_TRUE(config.IsLoaded());
}

TEST(ConfigLoadTests, MultipleFileLoad)
{
    auto a =
        CreateConfig(
            "config_a.cfg",
            "a=1\n");

    auto b =
        CreateConfig(
            "config_b.cfg",
            "b=2\n");

    ConfigSystem config;

    EXPECT_TRUE(config.Load({a, b}));
}

TEST(ConfigLoadTests, ReloadRejected)
{
    auto path =
        CreateConfig(
            "reload.cfg",
            "value=test\n");

    ConfigSystem config;

    EXPECT_TRUE(config.Load({path}));

    EXPECT_FALSE(config.Load({path}));
}

TEST(ConfigLoadTests, ClearAndReload)
{
    auto path =
        CreateConfig(
            "reload2.cfg",
            "value=test\n");

    ConfigSystem config;

    EXPECT_TRUE(config.Load({path}));

    config.Clear();

    EXPECT_FALSE(config.IsLoaded());

    EXPECT_TRUE(config.Load({path}));
}