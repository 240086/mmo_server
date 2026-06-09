#include <gtest/gtest.h>

#include <mmo/infrastructure/memory_pool/FixedBlockPool.hpp>

using namespace mmo::infrastructure::memory_pool;

TEST(MemoryPoolBoundaryTests, RejectMisalignedPointer)
{
    FixedBlockPool pool(64, 4);

    auto block = pool.Allocate();

    MemoryBlock invalid{
        static_cast<std::byte *>(block.data) + 1,
        block.size};

    EXPECT_FALSE(pool.Deallocate(invalid));
}

TEST(MemoryPoolBoundaryTests, RejectWrongSizeBlock)
{
    FixedBlockPool pool(64, 4);

    auto block = pool.Allocate();

    block.size = 128;

    EXPECT_FALSE(pool.Deallocate(block));
}

TEST(MemoryPoolBoundaryTests, RejectPointerOutsidePoolRange)
{
    FixedBlockPool pool(64, 4);

    int dummy = 0;

    MemoryBlock foreign{
        &dummy,
        64};

    EXPECT_FALSE(pool.Deallocate(foreign));
}

TEST(MemoryPoolBoundaryTests, RejectDoubleFree)
{
    FixedBlockPool pool(64, 4);

    auto block = pool.Allocate();

    EXPECT_TRUE(pool.Deallocate(block));

    EXPECT_FALSE(pool.Deallocate(block));
}

TEST(MemoryPoolBoundaryTests, ExhaustAndRecoverPool)
{
    FixedBlockPool pool(64, 8);

    std::vector<MemoryBlock> blocks;

    for (std::size_t i = 0; i < 8; ++i)
    {
        blocks.push_back(pool.Allocate());
    }

    EXPECT_EQ(pool.AvailableBlocks(), 0u);

    EXPECT_FALSE(pool.Allocate().IsValid());

    for (auto &block : blocks)
    {
        EXPECT_TRUE(pool.Deallocate(block));
    }

    EXPECT_EQ(pool.AvailableBlocks(), 8u);

    for (std::size_t i = 0; i < 8; ++i)
    {
        EXPECT_TRUE(pool.Allocate().IsValid());
    }
}

TEST(MemoryPoolBoundaryTests, AlignmentGuarantee)
{
    FixedBlockPool pool(
        sizeof(std::max_align_t),
        16);

    auto block = pool.Allocate();

    ASSERT_TRUE(block.IsValid());

    const auto address =
        reinterpret_cast<std::uintptr_t>(block.data);

    EXPECT_EQ(
        address % alignof(std::max_align_t),
        0u);
}
