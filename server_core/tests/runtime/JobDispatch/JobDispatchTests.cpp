#include "JobDispatchTestCommon.hpp"

#include <atomic>
#include <mutex>

#include "mmo/runtime/JobDispatch/JobDispatch.hpp"

namespace mmo::runtime::test
{
    TEST(JobDispatchTests,
         SingleNodeExecutesSuccessfully)
    {
        TaskRegistry registry;

        auto graph =
            CreateSingleNodeGraph();

        std::atomic_uint32_t counter{0};

        registry.Register(
            0,
            [&](RuntimeContext &) noexcept
            {
                counter.fetch_add(1);
            });

        RuntimeContext context;

        JobDispatch dispatcher;

        ASSERT_TRUE(
            dispatcher.Initialize(
                graph,
                context,
                registry));

        ASSERT_TRUE(
            dispatcher.ExecutePhase(
                RuntimePhaseId::Input));

        EXPECT_EQ(
            counter.load(),
            1u);

        dispatcher.Shutdown();
    }

    TEST(JobDispatchTests,
         LinearDependencyExecutesInOrder)
    {
        TaskRegistry registry;

        auto graph =
            CreateLinearGraph();

        ExecutionRecorder recorder;

        registry.Register(
            0,
            [&](RuntimeContext &) noexcept
            {
                recorder.Record(0);
            });

        registry.Register(
            1,
            [&](RuntimeContext &) noexcept
            {
                recorder.Record(1);
            });

        registry.Register(
            2,
            [&](RuntimeContext &) noexcept
            {
                recorder.Record(2);
            });

        RuntimeContext context;

        JobDispatch dispatcher;

        ASSERT_TRUE(
            dispatcher.Initialize(
                graph,
                context,
                registry));

        ASSERT_TRUE(
            dispatcher.ExecutePhase(
                RuntimePhaseId::Input));

        ASSERT_EQ(
            recorder.executionOrder.size(),
            3u);

        EXPECT_EQ(
            recorder.executionOrder[0],
            0);

        EXPECT_EQ(
            recorder.executionOrder[1],
            1);

        EXPECT_EQ(
            recorder.executionOrder[2],
            2);

        dispatcher.Shutdown();
    }

    TEST(JobDispatchTests,
         CrossPhaseDependencyRespected)
    {
        TaskGraph graph;

        graph.nodeCount = 2;

        graph.nodes[0].id = 0;
        graph.nodes[0].phaseId =
            RuntimePhaseId::Input;
        graph.nodes[0].parentCount = 0;
        graph.nodes[0].childCount = 1;
        graph.nodes[0].dependencyOffset = 0;

        graph.nodes[1].id = 1;
        graph.nodes[1].phaseId =
            RuntimePhaseId::Simulation;
        graph.nodes[1].parentCount = 1;

        graph.childTaskIds[0] = 1;

        graph.phaseRanges[static_cast<size_t>(
            RuntimePhaseId::Input)] =
            {
                0,
                1};

        graph.phaseRanges[static_cast<size_t>(
            RuntimePhaseId::Simulation)] =
            {
                1,
                1};

        graph.taskIdToNodeIndex[0] = 0;
        graph.taskIdToNodeIndex[1] = 1;

        TaskRegistry registry;

        std::atomic_bool aExecuted{false};
        std::atomic_bool bExecuted{false};

        registry.Register(
            0,
            [&](RuntimeContext &) noexcept
            {
                aExecuted.store(true);
            });

        registry.Register(
            1,
            [&](RuntimeContext &) noexcept
            {
                bExecuted.store(true);
            });

        RuntimeContext context;

        JobDispatch dispatcher;

        ASSERT_TRUE(
            dispatcher.Initialize(
                graph,
                context,
                registry));

        dispatcher.ExecutePhase(
            RuntimePhaseId::Input);

        EXPECT_TRUE(
            aExecuted.load());

        EXPECT_FALSE(
            bExecuted.load());

        dispatcher.ExecutePhase(
            RuntimePhaseId::Simulation);

        EXPECT_TRUE(
            bExecuted.load());

        dispatcher.Shutdown();
    }

    TEST(JobDispatchTests,
         ShutdownThenReinitialize)
    {
        TaskRegistry registry;

        auto graph =
            CreateSingleNodeGraph();

        registry.Register(
            0,
            [](RuntimeContext &) noexcept {});

        RuntimeContext context;

        JobDispatch dispatcher;

        ASSERT_TRUE(
            dispatcher.Initialize(
                graph,
                context,
                registry));

        dispatcher.Shutdown();

        ASSERT_TRUE(
            dispatcher.Initialize(
                graph,
                context,
                registry));

        dispatcher.Shutdown();
    }

    TEST(JobDispatchTests,
         MassiveParallelGraph)
    {
        constexpr uint32_t NodeCount = 128;

        TaskGraph graph;

        graph.nodeCount = NodeCount;

        graph.nodes[0].id = 0;
        graph.nodes[0].parentCount = 0;
        graph.nodes[0].childCount = NodeCount - 1;
        graph.nodes[0].dependencyOffset = 0;
        graph.nodes[0].phaseId = RuntimePhaseId::Input;

        for (uint32_t i = 1; i < NodeCount; ++i)
        {
            graph.nodes[i].id = i;
            graph.nodes[i].parentCount = 1;
            graph.nodes[i].phaseId =
                RuntimePhaseId::Input;

            graph.childTaskIds[i - 1] = i;

            graph.taskIdToNodeIndex[i] = i;
        }

        graph.taskIdToNodeIndex[0] = 0;

        graph.phaseRanges[static_cast<size_t>(
            RuntimePhaseId::Input)] =
            {
                0,
                NodeCount};

        TaskRegistry registry;

        std::atomic_uint32_t counter{0};

        for (uint32_t i = 0; i < NodeCount; ++i)
        {
            registry.Register(
                i,
                [&](RuntimeContext &) noexcept
                {
                    counter.fetch_add(
                        1,
                        std::memory_order_relaxed);
                });
        }

        RuntimeContext context;

        JobDispatch dispatcher;

        ASSERT_TRUE(
            dispatcher.Initialize(
                graph,
                context,
                registry));

        ASSERT_TRUE(
            dispatcher.ExecutePhase(
                RuntimePhaseId::Input));

        EXPECT_EQ(
            counter.load(),
            NodeCount);

        dispatcher.Shutdown();
    }

}