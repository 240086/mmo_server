// /home/xj/projects/mmo_server/server_core/tests/infrastructure/Net/NetServerConnectionTests.cpp
#include <gtest/gtest.h>
#include <boost/asio.hpp>
#include <vector>
#include <memory>
#include <mmo/infrastructure/Net/NetServer.hpp>
#include "NetTestHelper.hpp"

using namespace mmo::infrastructure::net;

TEST(NetServerConnectionTests, ManyConnections)
{
    NetServer server;
    ASSERT_TRUE(server.Start(35004));

    boost::asio::io_context io;
    std::vector<std::unique_ptr<boost::asio::ip::tcp::socket>> clients;

    constexpr int Count = 100;
    for (int i = 0; i < Count; ++i)
    {
        auto socket = std::make_unique<boost::asio::ip::tcp::socket>(io);
        socket->connect({boost::asio::ip::address_v4::loopback(), 35004});
        clients.push_back(std::move(socket));
    }

    // 🌟 动态等待 100 个客户端并发 Accept 全部就绪
    ASSERT_TRUE(test::WaitForCondition([&]()
                                       { return server.ConnectionCount() == Count; }));
    EXPECT_EQ(server.ConnectionCount(), Count);

    clients.clear(); // 瞬间切断所有连接，轰炸 `RemoveConnection` 锁

    // 🌟 动态等待服务器完成全部连接的优雅擦除，验证内存与生命周期零泄漏
    ASSERT_TRUE(test::WaitForCondition([&]()
                                       { return server.ConnectionCount() == 0u; }));
    EXPECT_EQ(server.ConnectionCount(), 0u);

    server.Stop();
}