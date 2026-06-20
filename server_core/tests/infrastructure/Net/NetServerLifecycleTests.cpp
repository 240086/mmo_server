// /home/xj/projects/mmo_server/server_core/tests/infrastructure/Net/NetServerLifecycleTests.cpp
#include <gtest/gtest.h>

#include <mmo/infrastructure/Net/NetServer.hpp>

using namespace mmo::infrastructure::net;

TEST(NetServerLifecycleTests, StartStop)
{
    NetServer server;

    ASSERT_TRUE(
        server.Start(25000));

    EXPECT_TRUE(
        server.IsRunning());

    server.Stop();

    EXPECT_FALSE(
        server.IsRunning());
}

TEST(NetServerLifecycleTests, DoubleStartRejected)
{
    NetServer server;

    ASSERT_TRUE(
        server.Start(25001));

    EXPECT_FALSE(
        server.Start(25001));

    server.Stop();
}

TEST(NetServerLifecycleTests, DoubleStopSafe)
{
    NetServer server;

    ASSERT_TRUE(
        server.Start(25002));

    server.Stop();

    EXPECT_NO_THROW(
        server.Stop());
}