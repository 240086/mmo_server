// /home/xj/projects/mmo_server/server_core/infrastructure/memory_pool/include/mmo/infrastructure/memory_pool/IMemoryPool.hpp
#pragma once

#include <mmo/infrastructure/memory_pool/MemoryBlock.hpp>

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

        // 💡 架构师微调：由于 MemoryBlock 仅 16 字节且轻量，传值（By Value）比传引用性能更优
        virtual bool Deallocate(MemoryBlock block) = 0;

        // 🛡️ 工业级防御：用于判定内存块是否属于本池，拦截 Double Free 与非法野指针
        [[nodiscard]]
        virtual bool Owns(MemoryBlock block) const noexcept = 0;

        [[nodiscard]]
        virtual std::size_t BlockSize() const noexcept = 0;

        [[nodiscard]]
        virtual std::size_t Capacity() const noexcept = 0;

        [[nodiscard]]
        virtual std::size_t AvailableBlocks() const noexcept = 0;
    };
}