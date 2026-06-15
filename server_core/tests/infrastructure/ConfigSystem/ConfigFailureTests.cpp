// /home/xj/projects/mmo_server/server_core/tests/infrastructure/ConfigSystem/ConfigFailureTests.cpp
#include <gtest/gtest.h>

#include <mmo/infrastructure/ConfigSystem/ConfigSystem.hpp>

using namespace mmo::infrastructure::config;

TEST(ConfigFailureTests, MissingFile)
{
    ConfigSystem config;

    EXPECT_FALSE(
        config.Load({"not_exist.cfg"}));
}

TEST(ConfigFailureTests, FailedLoadDoesNotMarkLoaded)
{
    ConfigSystem config;

    EXPECT_FALSE(
        config.Load({"invalid.cfg"}));

    EXPECT_FALSE(
        config.IsLoaded());
}