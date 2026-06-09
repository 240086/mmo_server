#include <gtest/gtest.h>

#include <vector>

#include <mmo/infrastructure/memory_pool/FixedBlockPool.hpp>

using namespace mmo::infrastructure::memory_pool;

TEST(MemoryPoolStressTests, LargeAllocateFreeCycle)
{
    constexpr std::size_t BlockCount = 1024;

    FixedBlockPool pool(64, BlockCount);

    for (std::size_t iteration = 0;
         iteration < 10000;
         ++iteration)
    {
        std::vector<MemoryBlock> blocks;

        blocks.reserve(BlockCount);

        for (std::size_t i = 0;
             i < BlockCount;
             ++i)
        {
            auto block = pool.Allocate();

            ASSERT_TRUE(block.IsValid());

            blocks.push_back(block);
        }

        ASSERT_EQ(
            pool.AvailableBlocks(),
            0u);

        for (auto &block : blocks)
        {
            ASSERT_TRUE(
                pool.Deallocate(block));
        }

        ASSERT_EQ(
            pool.AvailableBlocks(),
            BlockCount);
    }
}

TEST(MemoryPoolStressTests, FullExhaustionAndRecovery)
{
    constexpr std::size_t BlockCount = 1024;

    FixedBlockPool pool(
        64,
        BlockCount);

    std::vector<MemoryBlock> blocks;
    blocks.reserve(BlockCount);

    for (std::size_t i = 0; i < BlockCount; ++i)
    {
        auto block = pool.Allocate();

        ASSERT_TRUE(block.IsValid());

        blocks.push_back(block);
    }

    EXPECT_EQ(
        pool.AvailableBlocks(),
        0u);

    auto exhausted =
        pool.Allocate();

    EXPECT_FALSE(exhausted.IsValid());

    for (auto block : blocks)
    {
        EXPECT_TRUE(
            pool.Deallocate(block));
    }

    EXPECT_EQ(
        pool.AvailableBlocks(),
        BlockCount);

    for (std::size_t i = 0; i < BlockCount; ++i)
    {
        auto block = pool.Allocate();

        EXPECT_TRUE(
            block.IsValid());
    }

    EXPECT_EQ(
        pool.AvailableBlocks(),
        0u);
}