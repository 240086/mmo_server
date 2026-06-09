// /home/xj/projects/mmo_server/server_core/infrastructure/memory_pool/src/FixedBlockPool.cpp
#include <stdexcept>

#include <mmo/infrastructure/memory_pool/FixedBlockPool.hpp>

namespace mmo::infrastructure::memory_pool
{
    FixedBlockPool::FixedBlockPool(
        std::size_t blockSize,
        std::size_t blockCount)
        : m_storage(nullptr), m_freeList(), m_allocated(), m_blockSize(blockSize), m_blockCount(blockCount)
    {
        if (blockSize == 0)
        {
            throw std::invalid_argument(
                "FixedBlockPool: blockSize must be > 0");
        }

        if (blockCount == 0)
        {
            throw std::invalid_argument(
                "FixedBlockPool: blockCount must be > 0");
        }

        const std::size_t totalSize =
            blockSize * blockCount;

        m_storage =
            std::make_unique<std::byte[]>(totalSize);

        m_freeList.reserve(blockCount);

        m_allocated.resize(blockCount, 0);

        for (std::size_t i = 0; i < blockCount; ++i)
        {
            m_freeList.push_back(blockCount - 1 - i);
        }
    }

    MemoryBlock FixedBlockPool::Allocate()
    {
        if (m_freeList.empty())
        {
            return {};
        }

        const std::size_t index =
            m_freeList.back();

        m_freeList.pop_back();

        m_allocated[index] = 1;

        return MemoryBlock{
            m_storage.get() + index * m_blockSize,
            m_blockSize};
    }

    bool FixedBlockPool::Deallocate(
        MemoryBlock block)
    {
        if (!Owns(block))
        {
            return false;
        }

        auto *storageBegin = m_storage.get();

        auto offset =
            static_cast<std::byte *>(block.data) - storageBegin;

        if (offset % m_blockSize != 0)
        {
            return false;
        }

        const std::size_t index =
            static_cast<std::size_t>(offset) / m_blockSize;

        if (m_allocated[index] == 0)
        {
            // 🌟 契约完美兑现：如果该块已经归还过了，直接返回 false，守住 FreeList 一致性底线
            return false;
        }

        m_allocated[index] = 0;
        m_freeList.push_back(index);

        return true;
    }

    bool FixedBlockPool::Owns(
        MemoryBlock block) const noexcept
    {
        if (!block.IsValid())
        {
            return false;
        }

        auto *begin = m_storage.get();
        auto *end =
            begin + (m_blockSize * m_blockCount);

        auto *ptr =
            static_cast<std::byte *>(block.data);

        return ptr >= begin &&
               ptr < end &&
               block.size == m_blockSize;
    }

    std::size_t FixedBlockPool::BlockSize() const noexcept
    {
        return m_blockSize;
    }

    std::size_t FixedBlockPool::Capacity() const noexcept
    {
        return m_blockCount;
    }

    std::size_t FixedBlockPool::AvailableBlocks() const noexcept
    {
        return m_freeList.size();
    }
}