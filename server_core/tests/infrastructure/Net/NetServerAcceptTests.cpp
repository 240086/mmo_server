// /home/xj/projects/mmo_server/server_core/tests/infrastructure/Net/NetServerAcceptTests.cpp
#include <gtest/gtest.h>
#include <boost/asio.hpp>
#include <mmo/infrastructure/Net/NetServer.hpp>
#include "NetTestHelper.hpp"

using namespace mmo::infrastructure::net;

TEST(NetServerAcceptTests, AcceptSingleConnection)
{
    NetServer server;
    ASSERT_TRUE(server.Start(35001));

    boost::asio::io_context io;
    boost::asio::ip::tcp::socket client(io);

    client.connect({boost::asio::ip::address_v4::loopback(), 35001});

    // 🌟 消除 sleep，确定性等待连接数归一
    ASSERT_TRUE(test::WaitForCondition([&]()
                                       { return server.ConnectionCount() == 1u; }));
    EXPECT_EQ(server.ConnectionCount(), 1u);

    client.close();

    // 🌟 确定性等待连接被完全回收
    ASSERT_TRUE(test::WaitForCondition([&]()
                                       { return server.ConnectionCount() == 0u; }));
    EXPECT_EQ(server.ConnectionCount(), 0u);

    server.Stop();
}