#include <gtest/gtest.h>

#include <boost/asio.hpp>

#include <mmo/infrastructure/Net/NetServer.hpp>
#include <mmo/infrastructure/Net/PacketCodec.hpp>
#include <mmo/infrastructure/packet/PacketTypes.hpp>

#include "NetTestHelper.hpp"

using namespace mmo::infrastructure::net;
using namespace mmo::infrastructure::packet;

TEST(NetServerReceiveStressTests,
     ReceiveOneThousandPackets)
{
    constexpr int PacketCount = 1000;

    NetServer server;

    ASSERT_TRUE(
        server.Start(35006));

    boost::asio::io_context io;

    boost::asio::ip::tcp::socket client(io);

    client.connect(
        {boost::asio::ip::address_v4::loopback(),
         35006});

    for (int i = 0;
         i < PacketCount;
         ++i)
    {
        auto bytes =
            PacketCodec::Encode(
                static_cast<PacketId>(i),
                {1, 2, 3, 4});

        boost::asio::write(
            client,
            boost::asio::buffer(bytes));
    }

    std::vector<bool> received(
        static_cast<std::size_t>(
            PacketCount),
        false);

    int count = 0;

    bool success =
        test::WaitForCondition(
            [&]() -> bool // 💡 显式指定返回类型，防止编译器胡思乱想
            {
                ReceivedPacket packet;

                while (server.PollPacket(packet))
                {
                    // 🎯 关键修改：ASSERT_LT 改为 EXPECT_LT
                    EXPECT_LT(
                        packet.packetId,
                        PacketCount);

                    received[static_cast<std::size_t>(packet.packetId)] = true;

                    ++count;
                }

                return count == PacketCount;
            },
            std::chrono::seconds(5));

    ASSERT_TRUE(success);

    EXPECT_EQ(
        count,
        PacketCount);

    for (bool value : received)
    {
        EXPECT_TRUE(value);
    }

    server.Stop();
}