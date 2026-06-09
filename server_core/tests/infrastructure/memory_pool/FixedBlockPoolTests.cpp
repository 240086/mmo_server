// /home/xj/projects/mmo_server/server_core/tests/infrastructure/memory_pool/FixedBlockPoolTests.cpp
#include <gtest/gtest.h>

#include <mmo/infrastructure/memory_pool/FixedBlockPool.hpp>

using namespace mmo::infrastructure::memory_pool;

namespace
{
    constexpr std::size_t kBlockSize = 64;
    constexpr std::size_t kBlockCount = 8;
}

TEST(FixedBlockPoolTests, ConstructPool)
{
    FixedBlockPool pool(
        kBlockSize,
        kBlockCount);

    EXPECT_EQ(pool.BlockSize(), kBlockSize);
    EXPECT_EQ(pool.Capacity(), kBlockCount);
    EXPECT_EQ(pool.AvailableBlocks(), kBlockCount);
}

TEST(FixedBlockPoolTests, RejectZeroBlockSize)
{
    EXPECT_THROW(
        FixedBlockPool(0, 8),
        std::invalid_argument);
}

TEST(FixedBlockPoolTests, RejectZeroBlockCount)
{
    EXPECT_THROW(
        FixedBlockPool(64, 0),
        std::invalid_argument);
}

TEST(FixedBlockPoolTests, AllocateReturnsValidBlock)
{
    FixedBlockPool pool(
        kBlockSize,
        kBlockCount);

    auto block = pool.Allocate();

    EXPECT_TRUE(block.IsValid());
}

TEST(FixedBlockPoolTests, AllocateReturnsCorrectBlockSize)
{
    FixedBlockPool pool(
        kBlockSize,
        kBlockCount);

    auto block = pool.Allocate();

    EXPECT_EQ(block.size, kBlockSize);
}

TEST(FixedBlockPoolTests, AllocateConsumesAvailableBlock)
{
    FixedBlockPool pool(
        kBlockSize,
        kBlockCount);

    auto before = pool.AvailableBlocks();

    pool.Allocate();

    EXPECT_EQ(
        pool.AvailableBlocks(),
        before - 1);
}

TEST(FixedBlockPoolTests, AllocationFailsWhenPoolExhausted)
{
    FixedBlockPool pool(
        kBlockSize,
        kBlockCount);

    for (std::size_t i = 0; i < kBlockCount; ++i)
    {
        auto block = pool.Allocate();

        ASSERT_TRUE(block.IsValid());
    }

    auto exhausted = pool.Allocate();

    EXPECT_FALSE(exhausted.IsValid());
}

TEST(FixedBlockPoolTests, DeallocateReturnsBlockToPool)
{
    FixedBlockPool pool(
        kBlockSize,
        kBlockCount);

    auto block = pool.Allocate();

    auto before =
        pool.AvailableBlocks();

    EXPECT_TRUE(
        pool.Deallocate(block));

    EXPECT_EQ(
        pool.AvailableBlocks(),
        before + 1);
}

TEST(FixedBlockPoolTests, DeallocateRejectsInvalidBlock)
{
    FixedBlockPool pool(
        kBlockSize,
        kBlockCount);

    MemoryBlock invalid;

    EXPECT_FALSE(
        pool.Deallocate(invalid));
}

TEST(FixedBlockPoolTests, DeallocateRejectsForeignBlock)
{
    FixedBlockPool pool(
        kBlockSize,
        kBlockCount);

    std::byte external[64];

    MemoryBlock foreign{
        external,
        sizeof(external)};

    EXPECT_FALSE(
        pool.Deallocate(foreign));
}

TEST(FixedBlockPoolTests, OwnsAllocatedBlock)
{
    FixedBlockPool pool(
        kBlockSize,
        kBlockCount);

    auto block = pool.Allocate();

    EXPECT_TRUE(
        pool.Owns(block));
}

TEST(FixedBlockPoolTests, OwnsRejectsForeignBlock)
{
    FixedBlockPool pool(
        kBlockSize,
        kBlockCount);

    std::byte external[64];

    MemoryBlock foreign{
        external,
        sizeof(external)};

    EXPECT_FALSE(
        pool.Owns(foreign));
}

TEST(FixedBlockPoolTests, FreedBlockCanBeReallocated)
{
    FixedBlockPool pool(
        kBlockSize,
        kBlockCount);

    auto block1 =
        pool.Allocate();

    ASSERT_TRUE(
        pool.Deallocate(block1));

    auto block2 =
        pool.Allocate();

    EXPECT_EQ(
        block1.data,
        block2.data);
}

TEST(FixedBlockPoolTests, AvailableBlocksReflectPoolState)
{
    FixedBlockPool pool(
        kBlockSize,
        kBlockCount);

    auto a = pool.Allocate();
    auto b = pool.Allocate();

    EXPECT_EQ(
        pool.AvailableBlocks(),
        kBlockCount - 2);

    pool.Deallocate(a);
    pool.Deallocate(b);

    EXPECT_EQ(
        pool.AvailableBlocks(),
        kBlockCount);
}

TEST(FixedBlockPoolTests, RejectDoubleFree)
{
    FixedBlockPool pool(
        64,
        8);

    auto block =
        pool.Allocate();

    ASSERT_TRUE(
        pool.Deallocate(block));

    EXPECT_FALSE(
        pool.Deallocate(block));
}

TEST(FixedBlockPoolTests, RejectMisalignedPointer)
{
    FixedBlockPool pool(64, 4);

    auto block = pool.Allocate();

    MemoryBlock invalid{
        static_cast<std::byte *>(block.data) + 1,
        block.size};

    EXPECT_FALSE(pool.Deallocate(invalid));
}

