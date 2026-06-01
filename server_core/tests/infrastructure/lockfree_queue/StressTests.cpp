// #include <gtest/gtest.h>

// #include <atomic>
// #include <thread>
// #include <vector>

// #include <mmo/infrastructure/lockfree_queue/MPSCQueue.hpp>

// using namespace mmo::infrastructure::lockfree_queue;

// TEST(MPSCQueueStressTest, MassiveConcurrentTraffic)
// {
//     constexpr int kProducerCount = 4;
//     constexpr int kMessagesPerProducer = 250000;

//     constexpr int kTotalMessages =
//         kProducerCount * kMessagesPerProducer;

//     MPSCQueue<int, 65536> queue;

//     std::atomic<int> consumed = 0;

//     std::vector<std::thread> producers;

//     for (int producer = 0;
//          producer < kProducerCount;
//          ++producer)
//     {
//         producers.emplace_back([&, producer]()
//                                {
//             for (int i = 0;
//                  i < kMessagesPerProducer;
//                  ++i)
//             {
//                 int value =
//                     producer * 100000000 + i;

//                 while (!queue.Enqueue(value))
//                 {
//                     std::this_thread::yield();
//                 }
//             } });
//     }

//     std::thread consumer([&]()
//                          {
//         int value = 0;

//         while (consumed.load() < kTotalMessages)
//         {
//             if (queue.Dequeue(value))
//             {
//                 consumed.fetch_add(
//                     1,
//                     std::memory_order_relaxed);
//             }
//             else
//             {
//                 std::this_thread::yield();
//             }
//         } });

//     for (auto &producer : producers)
//     {
//         producer.join();
//     }

//     consumer.join();

//     ASSERT_EQ(consumed.load(), kTotalMessages);
// }