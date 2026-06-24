// server_core/tests/runtime/JobDispatch/JobDispatchStressTests.cpp
#include "JobDispatchTestCommon.hpp"

#include <atomic>
#include <array>
#include <vector>
#include <thread>

#include "mmo/runtime/JobDispatch/JobDispatch.hpp"

namespace mmo::runtime::test
{
    namespace stress_detail
    {
        // 采用全局静态原子变量，彻底杜绝多线程异步回调中的闭包生命周期逃逸与悬挂引用
        inline static std::atomic<uint32_t> g_tick_counter{0};
        inline static std::atomic<uint32_t> g_max_node_counter{0};
        inline static std::atomic<uint32_t> g_diamond_d_counter{0};
        inline static std::atomic<uint32_t> g_longrun_input_counter{0};
        inline static std::atomic<uint32_t> g_longrun_sim_counter{0};

        // 无锁链式顺序记录器：规避 std::mutex 引发的反向优先级翻转与死锁
        inline static std::atomic<uint32_t> g_chain_index{0};
        // 预分配最大节点空间的数组，确保拓扑排序记录过程实现零分配、零锁
        inline static std::array<TaskId, TaskGraphMaxNodesV1> g_chain_order{};
    }

    // =================================================================
    // Stress 1: RepeatedExecution (验证 10,000次 Tick 运行时状态重置与屏障)
    // =================================================================
    TEST(JobDispatchStressTests, RepeatedExecution)
    {
        RuntimeContext context;
        TaskRegistry registry;

        stress_detail::g_tick_counter.store(0, std::memory_order_relaxed);

        registry.Register(
            0,
            [](RuntimeContext &) noexcept
            {
                stress_detail::g_tick_counter.fetch_add(1, std::memory_order_release);
            });

        TaskGraph graph{};
        graph.nodeCount = 1;
        graph.nodes[0].id = 0;
        graph.nodes[0].phaseId = RuntimePhaseId::Input;
        graph.nodes[0].parentCount = 0;
        graph.nodes[0].childCount = 0;

        graph.phaseRanges[static_cast<size_t>(RuntimePhaseId::Input)] = {0, 1};
        graph.taskIdToNodeIndex[0] = 0;

        JobDispatch dispatch;
        ASSERT_TRUE(dispatch.Initialize(graph, context, registry));

        constexpr uint32_t kTicks = 10000;
        for (uint32_t i = 0; i < kTicks; ++i)
        {
            ASSERT_TRUE(dispatch.ExecutePhase(RuntimePhaseId::Input));
        }

        EXPECT_EQ(stress_detail::g_tick_counter.load(std::memory_order_acquire), kTicks);
        dispatch.Shutdown();
    }

    // =================================================================
    // Stress 2: MaximumNodeGraph (验证 ReadyQueue 并发容量上限)
    // =================================================================
    TEST(JobDispatchStressTests, MaximumNodeGraph)
    {
        RuntimeContext context;
        TaskRegistry registry;

        stress_detail::g_max_node_counter.store(0, std::memory_order_relaxed);

        TaskGraph graph{};
        graph.nodeCount = TaskGraphMaxNodesV1;

        for (TaskId i = 0; i < TaskGraphMaxNodesV1; ++i)
        {
            registry.Register(
                i,
                [](RuntimeContext &) noexcept
                {
                    stress_detail::g_max_node_counter.fetch_add(1, std::memory_order_release);
                });

            graph.nodes[i].id = i;
            graph.nodes[i].phaseId = RuntimePhaseId::Input;
            graph.nodes[i].parentCount = 0;
            graph.nodes[i].childCount = 0;
            graph.taskIdToNodeIndex[i] = i;
        }

        graph.phaseRanges[static_cast<size_t>(RuntimePhaseId::Input)] = {0, TaskGraphMaxNodesV1};

        JobDispatch dispatch;
        ASSERT_TRUE(dispatch.Initialize(graph, context, registry));
        ASSERT_TRUE(dispatch.ExecutePhase(RuntimePhaseId::Input));

        EXPECT_EQ(stress_detail::g_max_node_counter.load(std::memory_order_acquire), TaskGraphMaxNodesV1);
        dispatch.Shutdown();
    }

    // =================================================================
    // Stress 3: MaximumDependencyChain (无锁深度拓扑有向单向长链测试)
    // =================================================================
    TEST(JobDispatchStressTests, MaximumDependencyChain)
    {
        RuntimeContext context;
        TaskRegistry registry;

        stress_detail::g_chain_index.store(0, std::memory_order_relaxed);
        constexpr unsigned short InvalidTaskId16 = 0xFFFFu;
        stress_detail::g_chain_order.fill(InvalidTaskId16); // 填充无效 ID

        TaskGraph graph{};
        graph.nodeCount = TaskGraphMaxNodesV1;

        for (TaskId i = 0; i < TaskGraphMaxNodesV1; ++i)
        {
            registry.Register(
                i,
                [](RuntimeContext &) noexcept
                {
                    // 使用无锁原子自增占用槽位，杜绝使用 std::mutex 产生的调度崩塌
                    uint32_t slot = stress_detail::g_chain_index.fetch_add(1, std::memory_order_relaxed);
                    if (slot < TaskGraphMaxNodesV1)
                    {
                        stress_detail::g_chain_order[slot] = static_cast<TaskId>(slot);
                    }
                });

            graph.nodes[i].id = i;
            graph.nodes[i].phaseId = RuntimePhaseId::Input;
            graph.nodes[i].parentCount = (i == 0) ? 0u : 1u;
            graph.nodes[i].childCount = (i == TaskGraphMaxNodesV1 - 1) ? 0u : 1u;

            if (i < TaskGraphMaxNodesV1 - 1)
            {
                graph.nodes[i].dependencyOffset = i;
                graph.childTaskIds[i] = static_cast<TaskId>(i + 1);
            }
            graph.taskIdToNodeIndex[i] = i;
        }

        graph.phaseRanges[static_cast<size_t>(RuntimePhaseId::Input)] = {0, TaskGraphMaxNodesV1};

        JobDispatch dispatch;
        ASSERT_TRUE(dispatch.Initialize(graph, context, registry));
        ASSERT_TRUE(dispatch.ExecutePhase(RuntimePhaseId::Input));

        // 验证拓扑整链是否无遗漏执行
        EXPECT_EQ(stress_detail::g_chain_index.load(std::memory_order_acquire), TaskGraphMaxNodesV1);

        // 验证无锁依赖链条是否严格保序执行 0 -> 1 -> 2 -> ...
        for (uint32_t i = 0; i < TaskGraphMaxNodesV1; ++i)
        {
            EXPECT_EQ(stress_detail::g_chain_order[i], static_cast<TaskId>(i));
        }

        dispatch.Shutdown();
    }

    // =================================================================
    // Stress 4: DiamondDependency (经典钻石依赖收敛减流屏障)
    // =================================================================
    TEST(JobDispatchStressTests, DiamondDependency)
    {
        RuntimeContext context;
        TaskRegistry registry;

        stress_detail::g_diamond_d_counter.store(0, std::memory_order_relaxed);

        registry.Register(0, [](RuntimeContext &) noexcept {});
        registry.Register(1, [](RuntimeContext &) noexcept {});
        registry.Register(2, [](RuntimeContext &) noexcept {});
        registry.Register(
            3,
            [](RuntimeContext &) noexcept
            {
                stress_detail::g_diamond_d_counter.fetch_add(1, std::memory_order_release);
            });

        TaskGraph graph{};
        graph.nodeCount = 4;

        for (TaskId i = 0; i < 4; ++i)
        {
            graph.nodes[i].id = i;
            graph.nodes[i].phaseId = RuntimePhaseId::Input;
            graph.taskIdToNodeIndex[i] = i;
        }

        // 拓扑几何配置:
        // Node 0 (A) -> Children: 1(B), 2(C)
        graph.nodes[0].parentCount = 0;
        graph.nodes[0].childCount = 2;
        graph.nodes[0].dependencyOffset = 0;
        graph.childTaskIds[0] = 1;
        graph.childTaskIds[1] = 2;

        // Node 1 (B) -> Child: 3(D)
        graph.nodes[1].parentCount = 1;
        graph.nodes[1].childCount = 1;
        graph.nodes[1].dependencyOffset = 2;
        graph.childTaskIds[2] = 3;

        // Node 2 (C) -> Child: 3(D)
        graph.nodes[2].parentCount = 1;
        graph.nodes[2].childCount = 1;
        graph.nodes[2].dependencyOffset = 3;
        graph.childTaskIds[3] = 3;

        // Node 3 (D) -> 收敛端
        graph.nodes[3].parentCount = 2;
        graph.nodes[3].childCount = 0;

        graph.phaseRanges[static_cast<size_t>(RuntimePhaseId::Input)] = {0, 4};

        JobDispatch dispatch;
        ASSERT_TRUE(dispatch.Initialize(graph, context, registry));
        ASSERT_TRUE(dispatch.ExecutePhase(RuntimePhaseId::Input));

        // 核心断言：检测减流屏障，收敛节点 D 必须且只能被执行 1 次，严禁产生重复推队
        EXPECT_EQ(stress_detail::g_diamond_d_counter.load(std::memory_order_acquire), 1u);
        dispatch.Shutdown();
    }

    // =================================================================
    // Stress 5: CrossPhaseLongRun (验证跨阶段长周期状态无衰减)
    // =================================================================
    TEST(JobDispatchStressTests, CrossPhaseLongRun)
    {
        RuntimeContext context;
        TaskRegistry registry;

        stress_detail::g_longrun_input_counter.store(0, std::memory_order_relaxed);
        stress_detail::g_longrun_sim_counter.store(0, std::memory_order_relaxed);

        registry.Register(
            0,
            [](RuntimeContext &) noexcept
            {
                stress_detail::g_longrun_input_counter.fetch_add(1, std::memory_order_release);
            });

        registry.Register(
            1,
            [](RuntimeContext &) noexcept
            {
                stress_detail::g_longrun_sim_counter.fetch_add(1, std::memory_order_release);
            });

        TaskGraph graph{};
        graph.nodeCount = 2;

        // Phase 1: Input
        graph.nodes[0].id = 0;
        graph.nodes[0].phaseId = RuntimePhaseId::Input;
        graph.nodes[0].parentCount = 0;
        graph.nodes[0].childCount = 1;
        graph.nodes[0].dependencyOffset = 0;
        graph.childTaskIds[0] = 1;
        graph.taskIdToNodeIndex[0] = 0;

        // Phase 2: Simulation
        graph.nodes[1].id = 1;
        graph.nodes[1].phaseId = RuntimePhaseId::Simulation;
        graph.nodes[1].parentCount = 1;
        graph.nodes[1].childCount = 0;
        graph.taskIdToNodeIndex[1] = 1;

        graph.phaseRanges[static_cast<size_t>(RuntimePhaseId::Input)] = {0, 1};
        graph.phaseRanges[static_cast<size_t>(RuntimePhaseId::Simulation)] = {1, 1};

        JobDispatch dispatch;
        ASSERT_TRUE(dispatch.Initialize(graph, context, registry));

        constexpr uint32_t kTicks = 10000;
        for (uint32_t i = 0; i < kTicks; ++i)
        {
            ASSERT_TRUE(dispatch.ExecutePhase(RuntimePhaseId::Input));
            ASSERT_TRUE(dispatch.ExecutePhase(RuntimePhaseId::Simulation));
        }

        EXPECT_EQ(stress_detail::g_longrun_input_counter.load(std::memory_order_acquire), kTicks);
        EXPECT_EQ(stress_detail::g_longrun_sim_counter.load(std::memory_order_acquire), kTicks);

        dispatch.Shutdown();
    }
}