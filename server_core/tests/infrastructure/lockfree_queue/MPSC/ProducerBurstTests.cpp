// #include <gtest/gtest.h>

// #include <atomic>
// #include <thread>
// #include <vector>

// #include <mmo/infrastructure/lockfree_queue/MPSCQueue.hpp>

// using namespace mmo::infrastructure::lockfree_queue;

// TEST(MPSCQueueStressTest, BurstTrafficDoesNotDeadlock)
// {
//     constexpr int kBurstSize = 50000;

//     MPSCQueue<int, 65536> queue;

//     std::atomic<int> consumed = 0;

//     std::vector<std::thread> producers;

//     for (int p = 0; p < 8; ++p)
//     {
//         producers.emplace_back([&, p]()
//                                {
//             for (int i = 0; i < kBurstSize; ++i)
//             {
//                 while (!queue.Enqueue(i))
//                 {
//                     std::this_thread::yield();
//                 }
//             } });
//     }

//     std::thread consumer([&]()
//                          {
//         int value = 0;

//         while (consumed.load() <
//                kBurstSize * 8)
//         {
//             if (queue.Dequeue(value))
//             {
//                 consumed.fetch_add(1);
//             }
//         } });

//     for (auto &producer : producers)
//     {
//         producer.join();
//     }

//     consumer.join();

//     ASSERT_EQ(consumed.load(),
//               kBurstSize * 8);
// }