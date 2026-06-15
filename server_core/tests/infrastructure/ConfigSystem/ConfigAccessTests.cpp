// /home/xj/projects/mmo_server/server_core/tests/infrastructure/ConfigSystem/ConfigAccessTests.cpp
#include <gtest/gtest.h>

#include <fstream>

#include <mmo/infrastructure/ConfigSystem/ConfigSystem.hpp>

using namespace mmo::infrastructure::config;

TEST(ConfigAccessTests, GetString)
{
    auto path =
        std::filesystem::temp_directory_path() /
        "string.cfg";

    {
        std::ofstream f(path);
        f << "name=server\n";
    }

    ConfigSystem config;

    ASSERT_TRUE(config.Load({path}));

    EXPECT_EQ(
        config.Get<std::string>("name"),
        "server");
}

TEST(ConfigAccessTests, TryGetSuccess)
{
    auto path =
        std::filesystem::temp_directory_path() /
        "tryget.cfg";

    {
        std::ofstream f(path);
        f << "name=test\n";
    }

    ConfigSystem config;

    ASSERT_TRUE(config.Load({path}));

    auto *value =
        config.TryGet<std::string>("name");

    ASSERT_NE(value, nullptr);

    EXPECT_EQ(*value, "test");
}

TEST(ConfigAccessTests, TryGetMissing)
{
    ConfigSystem config;

    EXPECT_EQ(
        config.TryGet<std::string>("missing"),
        nullptr);
}

TEST(ConfigAccessTests, MissingKeyThrows)
{
    ConfigSystem config;

    EXPECT_THROW(
        config.Get<std::string>("missing"),
        ConfigNotFoundException);
}