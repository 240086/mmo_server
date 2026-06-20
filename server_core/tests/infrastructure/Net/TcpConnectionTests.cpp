// /home/xj/projects/mmo_server/server_core/tests/infrastructure/Net/TcpConnectionTests.cpp
#include <gtest/gtest.h>

#include <boost/asio.hpp>

#include <mmo/infrastructure/Net/TcpConnection.hpp>

using namespace mmo::infrastructure::net;

TEST(TcpConnectionTests, QueueFifoOrder)
{
    boost::asio::io_context io;

    TcpConnection::Socket socket(io);

    TcpConnection connection(
        1,
        std::move(socket));

    ASSERT_TRUE(
        connection.EnqueueSend(
            ByteBuffer{1}));

    ASSERT_TRUE(
        connection.EnqueueSend(
            ByteBuffer{2}));

    ByteBuffer out;

    ASSERT_TRUE(
        connection.TryPopSend(out));

    EXPECT_EQ(out[0], 1);

    ASSERT_TRUE(
        connection.TryPopSend(out));

    EXPECT_EQ(out[0], 2);
}

TEST(TcpConnectionTests, DisconnectChangesState)
{
    boost::asio::io_context io;

    TcpConnection::Socket socket(io);

    TcpConnection connection(
        1,
        std::move(socket));

    EXPECT_TRUE(
        connection.IsConnected());

    connection.Disconnect();

    EXPECT_FALSE(
        connection.IsConnected());
}

TEST(TcpConnectionTests, BackPressureLimit)
{
    boost::asio::io_context io;

    TcpConnection::Socket socket(io);

    TcpConnection connection(
        1,
        std::move(socket));

    for (
        std::size_t i = 0;
        i < TcpConnection::kMaxPendingSendPackets;
        ++i)
    {
        ASSERT_TRUE(
            connection.EnqueueSend(
                ByteBuffer{1}));
    }

    EXPECT_FALSE(
        connection.EnqueueSend(
            ByteBuffer{1}));
}