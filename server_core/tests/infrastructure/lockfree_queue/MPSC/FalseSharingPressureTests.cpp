// // /home/xj/projects/mmo_server/server_core/tests/infrastructure/lockfree_queue/FalseSharingPressureTests.cpp
// #include <gtest/gtest.h>
// #include <chrono>
// #include <thread>
// #include <vector>
// #include <atomic>
// #include <memory>

// #include <mmo/infrastructure/lockfree_queue/MPSCQueue.hpp>
// #include <mmo/infrastructure/lockfree_queue/CacheAligned.hpp>

// using namespace mmo::infrastructure::lockfree_queue;

// // ============================================================================
// // 测试维度一：编译期与运行期内存拓扑对齐审查 (Static & Layout Assertions)
// // ============================================================================

// TEST(FalseSharingPressureTest, CacheAlignedStructureVerification)
// {
//     // 1. 验证基础 CacheAligned 包装器的对齐严格符合硬件 CacheLine 大小 (64字节)
//     ASSERT_EQ(alignof(CacheAligned<int>), kCacheLineSize);
//     ASSERT_EQ(alignof(CacheAligned<uint64_t>), kCacheLineSize);

//     // 2. 核心 DSA 定律：无锁容器对象的尺寸必须是 CacheLine 的整数倍，防止数组排列时发生跨越边界的伪共享
//     ASSERT_EQ(sizeof(CacheAligned<uint64_t>) % kCacheLineSize, 0u);
//     ASSERT_EQ(sizeof(CacheAligned<int>) % kCacheLineSize, 0u);
// }

// TEST(FalseSharingPressureTest, QueueAlignmentVerification)
// {
//     // 验证 MPSCQueue 整体对齐属性已被正确提升至 64 字节
//     ASSERT_EQ(alignof(MPSCQueue<int, 1024>), kCacheLineSize);
// }

// // ============================================================================
// // 测试维度二：高频硬件 Cache-Bouncing 击穿压力测试 (Dynamic Throughput Test)
// // ============================================================================

// TEST(FalseSharingPressureTest, HighContentionCacheThroughput)
// {
//     // 增大容量以承载极端吞吐，必须为 2 的幂次 (2^16)
//     constexpr std::size_t kCapacity = 65536;
//     constexpr int kProducerCount = 3;
//     constexpr int kTotalOpsPerProducer = 3000000; // 每个生产者疯狂轰炸 300 万次 Enqueue

//     // 使用智能指针动态分配，避免大容量环形缓冲区直接撑爆测试栈空间
//     auto queue = std::make_unique<MPSCQueue<uint64_t, kCapacity>>();

//     std::atomic<bool> start_flag{false};
//     std::vector<std::thread> producers;
//     producers.reserve(kProducerCount);

//     // 1. 孵化多核生产者线程：疯狂抢占 m_enqueuePos
//     for (int i = 0; i < kProducerCount; ++i)
//     {
//         producers.emplace_back([&queue, &start_flag, kTotalOpsPerProducer](int id)
//                                {
//             // 栅栏同步：确保所有线程同时起跑，瞬间拉满 CPU 竞争压力
//             while (!start_flag.load(std::memory_order_relaxed)) {
//                 std::this_thread::yield();
//             }
            
//             for (uint64_t val = 0; val < kTotalOpsPerProducer; ++val) {
//                 // 混淆 Payload 写入，防止编译器将其作为死代码优化掉
//                 uint64_t payload = (static_cast<uint64_t>(id) << 32) | val;
                
//                 // 纯无锁自旋轰炸：不引入任何 Sleep，将生产者侧的 Cache Line 竞争推向极限
//                 while (!queue->Enqueue(payload)) {
// // 🧠 资深架构师推荐：现代跨平台硬件级指令退避
// #if defined(__x86_64__) || defined(_M_X64)
// #if defined(_MSC_VER)
//             _mm_pause();
// #else
//             __builtin_ia32_pause();
// #endif
// #elif defined(__ARM_ARCH) || defined(__aarch64__)
//         asm volatile("yield" ::: "memory");
// #else
//         std::this_thread::yield();
// #endif
// }
//             } }, i);
//     }

//     // 2. 消费者业务：在独立核心（当前主线程）高速消费
//     const std::size_t total_expected_messages = static_cast<std::size_t>(kProducerCount) * kTotalOpsPerProducer;
//     std::size_t consumed_count = 0;

//     // 时间戳锚定：发令枪响
//     auto start_time = std::chrono::high_resolution_clock::now();
//     start_flag.store(true, std::memory_order_release);

//     while (consumed_count < total_expected_messages)
//     {
//         uint64_t out_val = 0;
//         if (queue->Dequeue(out_val))
//         {
//             consumed_count++;
//         }
//         else
//         {
//             // 当队列打空时短暂出让，防止死锁，但在有数据时保持绝对的纯自旋
//             std::this_thread::yield();
//         }
//     }

//     auto end_time = std::chrono::high_resolution_clock::now();

//     // 收尾回收
//     for (auto &t : producers)
//     {
//         t.join();
//     }

//     // 3. 性能与架构指标审计
//     double duration_sec = std::chrono::duration<double>(end_time - start_time).count();
//     auto duration_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time).count();
//     double ops_per_sec = total_expected_messages / (duration_sec > 0.0 ? duration_sec : 0.001);
//     double million_ops_per_sec = ops_per_sec / 1000000.0;

//     // 输出微观硬核体检报告
//     std::cout << "\n[=== CACHE PERFORMANCE REPORT ===]\n"
//               << " Total Processed Messages : " << total_expected_messages << " ops\n"
//               << " Total Execution Time     : " << duration_ms << " ms\n"
//               << " Pure Cache Throughput    : " << million_ops_per_sec << " Million Ops/sec\n"
//               << "[================================]\n\n";

//     // 架构质量红线断言：
//     // 在现代多核 CPU（如 Intel/AMD/ARM）下，如果 alignas(64) 和防伪共享垫片完美生效，
//     // 单 Consumer 侧的私有变量 m_dequeuePos 绝不会受到多 Producer 频繁 CAS 产生的缓存污染。
//     // 该 MPSC 队列的纯内存吞吐量将轻松跨越 1000万次/秒 (10M Ops/sec) 的工业级门槛。
//     ASSERT_GT(million_ops_per_sec, 10.0);
// }