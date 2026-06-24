#include "JobDispatchTestCommon.hpp"

namespace mmo::runtime::test
{

    TEST(TaskRegistryTests, ValidateReturnsTrueWhenAllTasksRegistered)
    {
        TaskRegistry registry;

        auto graph = CreateLinearGraph();

        registry.Register(
            0,
            [](RuntimeContext &) noexcept {});

        registry.Register(
            1,
            [](RuntimeContext &) noexcept {});

        registry.Register(
            2,
            [](RuntimeContext &) noexcept {});

        EXPECT_TRUE(
            registry.Validate(graph));
    }

    TEST(TaskRegistryTests, ValidateReturnsFalseWhenTaskMissing)
    {
        TaskRegistry registry;

        auto graph = CreateLinearGraph();

        registry.Register(
            0,
            [](RuntimeContext &) noexcept {});

        registry.Register(
            1,
            [](RuntimeContext &) noexcept {});

        EXPECT_FALSE(
            registry.Validate(graph));
    }

    TEST(TaskRegistryTests, ResolveReturnsRegisteredFunction)
    {
        TaskRegistry registry;

        registry.Register(
            7,
            [](RuntimeContext &) noexcept {});

        EXPECT_NE(
            registry.Resolve(7),
            nullptr);
    }

    TEST(TaskRegistryTests, ResolveReturnsNullptrForUnknownTask)
    {
        TaskRegistry registry;

        EXPECT_EQ(
            registry.Resolve(999),
            nullptr);
    }

}