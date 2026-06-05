// /home/xj/projects/mmo_server/server_core/infrastructure/memory_pool/include/mmo/infrastructure/memory_pool/IMemoryPool.hpp
#pragma once

#include "MemoryBlock.hpp"

namespace mmo::infrastructure::memory_pool
{
    class IMemoryPool
    {
        // FixedBlockPool
        // LockFreePool
        // ThreadLocalPool
        // ArenaPool
    public:
        virtual ~IMemoryPool() = default;

        [[nodiscard]]
        virtual MemoryBlock Allocate() = 0;

        virtual bool Deallocate(const MemoryBlock &block) = 0;

        [[nodiscard]]
        virtual std::size_t BlockSize() const noexcept = 0;

        [[nodiscard]]
        virtual std::size_t Capacity() const noexcept = 0;

        [[nodiscard]]
        virtual std::size_t AvailableBlocks() const noexcept = 0;
    };
}