#include <gtest/gtest.h>

#include <type_traits>

#include <mmo/runtime/RuntimePipeline/RuntimePipeline.hpp>

using namespace mmo::runtime;

TEST(RuntimePipelineLayoutTests, ShouldBeStandardLayout)
{
    EXPECT_TRUE(
        std::is_standard_layout_v<
            RuntimePipeline>);
}

TEST(RuntimePipelineLayoutTests, ShouldBeTriviallyCopyable)
{
    EXPECT_TRUE(
        std::is_trivially_copyable_v<
            RuntimePipeline>);
}

TEST(RuntimePipelineLayoutTests, ShouldBeTriviallyDestructible)
{
    EXPECT_TRUE(
        std::is_trivially_destructible_v<
            RuntimePipeline>);
}

TEST(RuntimePipelineLayoutTests, ShouldNotContainVirtualTable)
{
    EXPECT_FALSE(
        std::has_virtual_destructor_v<
            RuntimePipeline>);
}

TEST(RuntimePipelineLayoutTests, SizeShouldRemainStable)
{
    EXPECT_LE(
        sizeof(RuntimePipeline),
        16u);
}