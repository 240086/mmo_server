// /home/xj/projects/mmo_server/server_core/tests/infrastructure/Net/NetServerReceiveTests.cpp
#include <gtest/gtest.h>
#include <boost/asio.hpp>
#include <mmo/infrastructure/Net/NetServer.hpp>
#include <mmo/infrastructure/Net/PacketCodec.hpp>
#include "NetTestHelper.hpp"

using namespace mmo::infrastructure::net;

TEST(NetServerReceiveTests, ReceivePacket)
{
    NetServer server;
    ASSERT_TRUE(server.Start(35002));

    boost::asio::io_context io;
    boost::asio::ip::tcp::socket client(io);

    client.connect({boost::asio::ip::address_v4::loopback(), 35002});

    auto bytes = PacketCodec::Encode(1001, {1, 2, 3, 4});
    boost::asio::write(client, boost::asio::buffer(bytes));

    ReceivedPacket packet;
    // 🌟 动态轮询直到 PollPacket 成功，对抗异步网络线程的非确定性调度
    bool success = test::WaitForCondition([&]()
                                          { return server.PollPacket(packet); });

    ASSERT_TRUE(success);
    EXPECT_EQ(packet.packetId, 1001);
    ASSERT_EQ(packet.payload.size(), 4u);
    EXPECT_EQ(packet.payload[0], 1);

    server.Stop();
}