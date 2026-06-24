/**
 * @file TaskGraphAbiTests.cpp
 * @brief TaskGraph 模块统一 ABI 契约、内存布局与静态构造测试
 * * 本测试核心目标在于死锁底层控制面的物理内存边界，防止任何运行期隐式退化。
 */

#include <gtest/gtest.h>
#include <type_traits>
#include <cstdint>

#include <mmo/runtime/TaskGraph/TaskGraphTypes.hpp>
#include <mmo/runtime/TaskGraph/TaskGraphConstants.hpp>
#include <mmo/runtime/TaskGraph/TaskDependency.hpp>
#include <mmo/runtime/TaskGraph/TaskNode.hpp>
#include <mmo/runtime/TaskGraph/TaskGraph.hpp>

namespace mmo::runtime::test
{

    // ============================================================================
    // 1. 基础元数据类型与常量测试 (Types & Constants Guarantees)
    // ============================================================================

    TEST(TaskGraphAbiTests, FundamentalTypesAndConstantsVerify)
    {
        // 保证 TaskId 类型稳定性，防止未来的隐式类型转换截断 Bug
        EXPECT_TRUE((std::is_same_v<TaskId, std::uint16_t>));
        EXPECT_EQ(InvalidTaskId, 0xFFFF);

        // 锁死容量边界，任何容量变动必须显式修改此处以通过审计
        EXPECT_EQ(TaskGraphMaxNodesV1, 256);
        EXPECT_EQ(TaskGraphMaxDependenciesV1, 1024);
    }

    // ============================================================================
    // 2. 内存布局与特性测试 (Standard Layout & Trivially Copyable)
    // ============================================================================

    TEST(TaskGraphAbiTests, StructureMemoryLayoutContract)
    {
        // 契约一：TaskDependency 必须保持标准布局且无虚表 overhead
        EXPECT_TRUE(std::is_standard_layout_v<TaskDependency>);
        EXPECT_TRUE(std::is_trivially_copyable_v<TaskDependency>);
        EXPECT_EQ(sizeof(TaskDependency), 4u); // TaskId(2B) + TaskId(2B)

        // 契约二：TaskNode 必须满足高性能 CSR 的 4 字节完美对齐布局
        EXPECT_TRUE(std::is_standard_layout_v<TaskNode>);
        EXPECT_TRUE(std::is_trivially_copyable_v<TaskNode>);
        EXPECT_TRUE(std::is_trivially_destructible_v<TaskNode>);
        EXPECT_FALSE(std::has_virtual_destructor_v<TaskNode>);
        EXPECT_EQ(sizeof(TaskNode), 12u); // TaskId(2B) + PhaseId(1B) + Pad(1B) + Offset(4B) + Count(4B)

        // 契约三：TaskGraph 本体必须作为完全扁平的结构化二进制块 (Blob)，支持 O(1) 网络零复制同步
        EXPECT_TRUE(std::is_standard_layout_v<TaskGraph>);
        EXPECT_TRUE(std::is_trivially_copyable_v<TaskGraph>);
        EXPECT_TRUE(std::is_trivially_destructible_v<TaskGraph>);

        // 精确锁死物理内存足迹：3072(Nodes) + 2(Count) + 2048(ChildIds) + 2(Count) = 5124 字节
        // 完美契合现代 CPU 的 L1 Data Cache (通常为 32KB~48KB)，允许整图常驻缓存
        EXPECT_EQ(sizeof(TaskGraph), 5124u);
    }

    // ============================================================================
    // 3. 安全零初始化与边界测试 (Initialization & Boundary Security)
    // ============================================================================

    TEST(TaskGraphAbiTests, SecureZeroInitialization)
    {
        // 验证默认构造是否安全执行了字段零初始化
        TaskGraph graph;
        EXPECT_EQ(graph.nodeCount, 0);
        EXPECT_EQ(graph.dependencyCount, 0);

        // 验证聚合初始化（零填充）行为
        TaskGraph aggregateGraph{};
        EXPECT_EQ(aggregateGraph.nodeCount, 0);
        EXPECT_EQ(aggregateGraph.dependencyCount, 0);

        // 随机抽查内部扁平数组的物理隔离区，确保没有未定义死值 (Garbage values)
        EXPECT_EQ(graph.nodes[0].id, InvalidTaskId);
        EXPECT_EQ(graph.nodes[TaskGraphMaxNodesV1 - 1].id, InvalidTaskId);
        EXPECT_EQ(graph.childTaskIds[0], 0u); // 默认初始化下底层元素置零
        EXPECT_EQ(graph.childTaskIds[TaskGraphMaxDependenciesV1 - 1], 0u);
    }

} // namespace mmo::runtime::test