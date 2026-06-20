// /home/xj/projects/mmo_server/server_core/tests/infrastructure/Net/NetServerLockFreeQueueTests.cpp
#include <gtest/gtest.h>
#include <boost/asio.hpp>
#include <mmo/infrastructure/Net/NetServer.hpp>
#include <mmo/infrastructure/Net/PacketCodec.hpp>
#include <mmo/infrastructure/packet/PacketTypes.hpp>
#include "NetTestHelper.hpp"

using namespace mmo::infrastructure::net;
using namespace mmo::infrastructure::packet;

TEST(NetServerLockFreeQueueTests, MultiplePackets)
{
    NetServer server;
    ASSERT_TRUE(server.Start(35003));

    boost::asio::io_context io;
    boost::asio::ip::tcp::socket client(io);
    client.connect({boost::asio::ip::address_v4::loopback(), 35003});

    constexpr int Count = 100;
    for (int i = 0; i < Count; ++i)
    {
        auto bytes = PacketCodec::Encode(static_cast<PacketId>(i), {1, 2, 3});
        boost::asio::write(client, boost::asio::buffer(bytes));
    }

    int received = 0;
    ReceivedPacket packet;

    // 🌟 持续收包，确保 100 个包无丢失，且彻底榨干无锁队列
    bool success = test::WaitForCondition([&]()
                                          {
        while (server.PollPacket(packet)) {
            EXPECT_EQ(packet.packetId, static_cast<PacketId>(received)); // 验证 FIFO 时序严格性
            ++received;
        }
        return received == Count; });

    ASSERT_TRUE(success);
    EXPECT_EQ(received, Count);

    server.Stop();
}