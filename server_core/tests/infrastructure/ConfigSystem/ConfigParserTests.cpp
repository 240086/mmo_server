// /home/xj/projects/mmo_server/server_core/tests/infrastructure/ConfigSystem/ConfigParserTests.cpp
#include <gtest/gtest.h>

#include <fstream>

#include <mmo/infrastructure/ConfigSystem/parser/KeyValueConfigParser.hpp>

using namespace mmo::infrastructure::config;

TEST(ConfigParserTests, EmptyFile)
{
    auto path =
        std::filesystem::temp_directory_path() /
        "empty.cfg";

    {
        std::ofstream{path};
    }

    KeyValueConfigParser parser;

    std::unordered_map<
        ConfigKey,
        ConfigValue>
        storage;

    auto result =
        parser.ParseFile(
            path,
            storage);

    EXPECT_TRUE(result.success);
}

TEST(ConfigParserTests, CommentLineIgnored)
{
    auto path =
        std::filesystem::temp_directory_path() /
        "comment.cfg";

    {
        std::ofstream f(path);

        f << "#comment\n";
        f << "name=test\n";
    }

    KeyValueConfigParser parser;

    std::unordered_map<
        ConfigKey,
        ConfigValue>
        storage;

    auto result =
        parser.ParseFile(
            path,
            storage);

    EXPECT_TRUE(result.success);

    EXPECT_EQ(
        std::get<std::string>(
            storage["name"]),
        "test");
}

TEST(ConfigParserTests, DuplicateKeyOverride)
{
    auto path =
        std::filesystem::temp_directory_path() /
        "dup.cfg";

    {
        std::ofstream f(path);

        f << "value=1\n";
        f << "value=2\n";
    }

    KeyValueConfigParser parser;

    std::unordered_map<
        ConfigKey,
        ConfigValue>
        storage;

    parser.ParseFile(path, storage);

    EXPECT_EQ(
        std::get<std::string>(
            storage["value"]),
        "2");
}