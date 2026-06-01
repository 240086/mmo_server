// #include <gtest/gtest.h>

// #include <atomic>
// #include <thread>
// #include <unordered_set>
// #include <vector>

// #include <mmo/infrastructure/lockfree_queue/MPSCQueue.hpp>

// using namespace mmo::infrastructure::lockfree_queue;

// TEST(MPSCQueueTest, MultiProducerNoLoss)
// {
//     constexpr int kProducerCount = 4;
//     constexpr int kMessagesPerProducer = 10000;

//     MPSCQueue<int, 65536> queue;

//     std::atomic<int> produced = 0;

//     std::vector<std::thread> producers;

//     for (int p = 0; p < kProducerCount; ++p)
//     {
//         producers.emplace_back([&, p]()
//                                {
//             for (int i = 0; i < kMessagesPerProducer; ++i)
//             {
//                 int value = p * 1000000 + i;

//                 while (!queue.Enqueue(value))
//                 {
//                 }

//                 produced.fetch_add(1, std::memory_order_relaxed);
//             } });
//     }

//     std::unordered_set<int> received;

//     while (received.size() <
//            static_cast<size_t>(kProducerCount * kMessagesPerProducer))
//     {
//         int value = 0;

//         if (queue.Dequeue(value))
//         {
//             received.insert(value);
//         }
//     }

//     for (auto &thread : producers)
//     {
//         thread.join();
//     }

//     ASSERT_EQ(
//         received.size(),
//         static_cast<size_t>(kProducerCount * kMessagesPerProducer));
// }