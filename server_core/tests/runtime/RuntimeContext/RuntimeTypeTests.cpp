#include <gtest/gtest.h>

#include <type_traits>

#include "mmo/runtime/RuntimeContext/RuntimeTypes.hpp"

using namespace mmo::runtime;

TEST(RuntimeTypeTests, TickIdShouldBeUint64)
{
    EXPECT_TRUE((std::is_same_v<TickId, std::uint64_t>));
}

TEST(RuntimeTypeTests, FrameIndexShouldBeUint64)
{
    EXPECT_TRUE((std::is_same_v<FrameIndex, std::uint64_t>));
}

TEST(RuntimeTypeTests, TickDurationShouldBeUint32)
{
    EXPECT_TRUE((std::is_same_v<TickDurationMs, std::uint32_t>));
}

TEST(RuntimeTypeTests, WorkerCountShouldBeUint32)
{
    EXPECT_TRUE((std::is_same_v<WorkerCount, std::uint32_t>));
}