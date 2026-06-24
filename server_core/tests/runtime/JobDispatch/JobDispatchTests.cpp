#include "JobDispatchTestCommon.hpp"

#include <atomic>
#include <mutex>
#include <vector>

#include "mmo/runtime/JobDispatch/JobDispatch.hpp"

namespace mmo::runtime::test
{
    // 改用标准的全局静态变量，以确保 Worker 线程的修改能同步到主线程。
    // 在每个 TEST 内部开头都会显式执行 store 重置，防止单测间相互污染。
    namespace detail
    {
        inline static std::atomic<uint32_t> g_single_counter{0};
        inline static std::atomic<ExecutionRecorder *> g_current_recorder{nullptr};
        inline static std::atomic_bool g_aExecuted{false};
        inline static std::atomic_bool g_bExecuted{false};
        inline static std::atomic_uint32_t g_massive_counter{0};
    }

    TEST(JobDispatchTests, SingleNodeExecutesSuccessfully)
    {
        TaskRegistry registry;
        auto graph = CreateSingleNodeGraph();

        // 显式重置状态
        detail::g_single_counter.store(0, std::memory_order_relaxed);

        registry.Register(
            0,
            [](RuntimeContext &) noexcept
            {
                detail::g_single_counter.fetch_add(1, std::memory_order_release);
            });

        RuntimeContext context;
        JobDispatch dispatcher;

        ASSERT_TRUE(dispatcher.Initialize(graph, context, registry));
        ASSERT_TRUE(dispatcher.ExecutePhase(RuntimePhaseId::Input));

        EXPECT_EQ(detail::g_single_counter.load(std::memory_order_acquire), 1u);

        dispatcher.Shutdown();
    }

    TEST(JobDispatchTests, LinearDependencyExecutesInOrder)
    {
        TaskRegistry registry;
        auto graph = CreateLinearGraph();

        ExecutionRecorder recorder;
        // 采用 release 语义发布指针
        detail::g_current_recorder.store(&recorder, std::memory_order_release);

        registry.Register(
            0,
            [](RuntimeContext &) noexcept
            {
                ExecutionRecorder *rec = detail::g_current_recorder.load(std::memory_order_acquire);
                if (rec)
                {
                    rec->Record(0);
                }
            });

        registry.Register(
            1,
            [](RuntimeContext &) noexcept
            {
                ExecutionRecorder *rec = detail::g_current_recorder.load(std::memory_order_acquire);
                if (rec)
                {
                    rec->Record(1);
                }
            });

        registry.Register(
            2,
            [](RuntimeContext &) noexcept
            {
                ExecutionRecorder *rec = detail::g_current_recorder.load(std::memory_order_acquire);
                if (rec)
                {
                    rec->Record(2);
                }
            });

        RuntimeContext context;
        JobDispatch dispatcher;

        ASSERT_TRUE(dispatcher.Initialize(graph, context, registry));
        ASSERT_TRUE(dispatcher.ExecutePhase(RuntimePhaseId::Input));

        ASSERT_EQ(recorder.executionOrder.size(), 3u);
        EXPECT_EQ(recorder.executionOrder[0], 0);
        EXPECT_EQ(recorder.executionOrder[1], 1);
        EXPECT_EQ(recorder.executionOrder[2], 2);

        dispatcher.Shutdown();
        detail::g_current_recorder.store(nullptr, std::memory_order_relaxed);
    }

    TEST(JobDispatchTests, CrossPhaseDependencyRespected)
    {
        TaskGraph graph{};

        graph.nodeCount = 2;

        graph.nodes[0].id = 0;
        graph.nodes[0].phaseId = RuntimePhaseId::Input;
        graph.nodes[0].parentCount = 0;
        graph.nodes[0].childCount = 1;
        graph.nodes[0].dependencyOffset = 0;

        graph.nodes[1].id = 1;
        graph.nodes[1].phaseId = RuntimePhaseId::Simulation;
        graph.nodes[1].parentCount = 1;

        graph.childTaskIds[0] = 1;

        graph.phaseRanges[static_cast<size_t>(RuntimePhaseId::Input)] = {0, 1};
        graph.phaseRanges[static_cast<size_t>(RuntimePhaseId::Simulation)] = {1, 1};

        graph.taskIdToNodeIndex[0] = 0;
        graph.taskIdToNodeIndex[1] = 1;

        TaskRegistry registry;

        detail::g_aExecuted.store(false, std::memory_order_relaxed);
        detail::g_bExecuted.store(false, std::memory_order_relaxed);

        registry.Register(
            0,
            [](RuntimeContext &) noexcept
            {
                detail::g_aExecuted.store(true, std::memory_order_release);
            });

        registry.Register(
            1,
            [](RuntimeContext &) noexcept
            {
                detail::g_bExecuted.store(true, std::memory_order_release);
            });

        RuntimeContext context;
        JobDispatch dispatcher;

        ASSERT_TRUE(dispatcher.Initialize(graph, context, registry));

        dispatcher.ExecutePhase(RuntimePhaseId::Input);
        EXPECT_TRUE(detail::g_aExecuted.load(std::memory_order_acquire));
        EXPECT_FALSE(detail::g_bExecuted.load(std::memory_order_acquire));

        dispatcher.ExecutePhase(RuntimePhaseId::Simulation);
        EXPECT_TRUE(detail::g_bExecuted.load(std::memory_order_acquire));

        dispatcher.Shutdown();
    }

    TEST(JobDispatchTests, ShutdownThenReinitialize)
    {
        TaskRegistry registry;
        auto graph = CreateSingleNodeGraph();

        registry.Register(0, [](RuntimeContext &) noexcept {});

        RuntimeContext context;
        JobDispatch dispatcher;

        ASSERT_TRUE(dispatcher.Initialize(graph, context, registry));
        dispatcher.Shutdown();

        ASSERT_TRUE(dispatcher.Initialize(graph, context, registry));
        dispatcher.Shutdown();
    }

    TEST(JobDispatchTests, MassiveParallelGraph)
    {
        constexpr uint32_t NodeCount = 128;
        TaskGraph graph{};

        graph.nodeCount = NodeCount;

        graph.nodes[0].id = 0;
        graph.nodes[0].parentCount = 0;
        graph.nodes[0].childCount = NodeCount - 1;
        graph.nodes[0].dependencyOffset = 0;
        graph.nodes[0].phaseId = RuntimePhaseId::Input;

        for (uint32_t i = 1; i < NodeCount; ++i)
        {
            graph.nodes[i].id = static_cast<TaskId>(i);
            graph.nodes[i].parentCount = 1;
            graph.nodes[i].phaseId = RuntimePhaseId::Input;

            graph.childTaskIds[i - 1] = static_cast<TaskId>(i);
            graph.taskIdToNodeIndex[i] = static_cast<TaskId>(i);
        }

        graph.taskIdToNodeIndex[0] = 0;
        graph.phaseRanges[static_cast<size_t>(RuntimePhaseId::Input)] = {0, NodeCount};

        TaskRegistry registry;
        detail::g_massive_counter.store(0, std::memory_order_relaxed);

        for (uint32_t i = 0; i < NodeCount; ++i)
        {
            registry.Register(
                static_cast<TaskId>(i),
                [](RuntimeContext &) noexcept
                {
                    detail::g_massive_counter.fetch_add(1, std::memory_order_release);
                });
        }

        RuntimeContext context;
        JobDispatch dispatcher;

        ASSERT_TRUE(dispatcher.Initialize(graph, context, registry));
        ASSERT_TRUE(dispatcher.ExecutePhase(RuntimePhaseId::Input));

        EXPECT_EQ(detail::g_massive_counter.load(std::memory_order_acquire), NodeCount);

        dispatcher.Shutdown();
    }
}