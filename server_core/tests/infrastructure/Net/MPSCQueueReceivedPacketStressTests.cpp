#include <gtest/gtest.h>

#include <atomic>
#include <thread>
#include <vector>

#include <mmo/infrastructure/lockfree_queue/MPSCQueue.hpp>
#include <mmo/infrastructure/Net/NetTypes.hpp>
#include <mmo/infrastructure/packet/PacketTypes.hpp>

using namespace mmo::infrastructure::net;
using namespace mmo::infrastructure::packet;
using namespace mmo::infrastructure::lockfree_queue;

TEST(MPSCQueueReceivedPacketStressTests, MultiProducerSingleConsumer)
{
    constexpr std::size_t QueueCapacity = 65536;

    constexpr int ProducerCount = 8;
    constexpr int PacketsPerProducer = 10000;

    constexpr int TotalPackets =
        ProducerCount * PacketsPerProducer;

    MPSCQueue<ReceivedPacket, QueueCapacity> queue;

    std::atomic<int> produced{0};
    std::atomic<int> consumed{0};

    std::vector<std::thread> producers;

    for (int producerId = 0;
         producerId < ProducerCount;
         ++producerId)
    {
        producers.emplace_back(
            [&queue,
             &produced,
             producerId]()
            {
                for (int i = 0;
                     i < PacketsPerProducer;
                     ++i)
                {
                    ReceivedPacket packet;

                    packet.connectionId =
                        static_cast<ConnectionId>(producerId);

                    packet.packetId =
                        static_cast<PacketId>(i);

                    packet.payload.resize(
                        static_cast<std::size_t>(
                            128 + (i % 2048)));

                    while (!queue.Enqueue(
                        std::move(packet)))
                    {
                        std::this_thread::yield();
                    }

                    produced.fetch_add(
                        1,
                        std::memory_order_relaxed);
                }
            });
    }

    std::thread consumer(
        [&]()
        {
            ReceivedPacket packet;

            while (consumed.load(
                       std::memory_order_relaxed) < TotalPackets)
            {
                if (queue.Dequeue(packet))
                {
                    ++consumed;

                    EXPECT_GE(
                        packet.payload.size(),
                        128u);
                }
                else
                {
                    std::this_thread::yield();
                }
            }
        });

    for (auto &thread : producers)
    {
        thread.join();
    }

    consumer.join();

    EXPECT_EQ(
        produced.load(),
        TotalPackets);

    EXPECT_EQ(
        consumed.load(),
        TotalPackets);
}