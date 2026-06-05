// /home/xj/projects/mmo_server/server_core/infrastructure/memory_pool/include/mmo/infrastructure/memory_pool/FixedBlockPool.hpp
#pragma once

#include <vector>

#include "IMemoryPool.hpp"

namespace mmo::infrastructure::memory_pool
{
    class FixedBlockPool final
        : public IMemoryPool
    {
    public:
        FixedBlockPool(
            std::size_t blockSize,
            std::size_t blockCount);

        MemoryBlock Allocate() override;

        bool Deallocate(
            const MemoryBlock &block) override;

        std::size_t BlockSize() const noexcept override;

        std::size_t Capacity() const noexcept override;

        std::size_t AvailableBlocks() const noexcept override;

    private:
        // 后续实现阶段再设计内部结构
    };
}