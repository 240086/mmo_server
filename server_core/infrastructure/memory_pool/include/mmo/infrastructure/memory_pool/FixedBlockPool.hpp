// /home/xj/projects/mmo_server/server_core/infrastructure/memory_pool/include/mmo/infrastructure/memory_pool/FixedBlockPool.hpp
#pragma once

#include <memory>
#include <vector>
#include <cstddef>
#include <cstdint>

#include <mmo/infrastructure/memory_pool/IMemoryPool.hpp>

namespace mmo::infrastructure::memory_pool
{
    class FixedBlockPool final
        : public IMemoryPool
    {
    public:
        FixedBlockPool(
            std::size_t blockSize,
            std::size_t blockCount);

        ~FixedBlockPool() override = default;

        // 禁止拷贝与移动，严格确保 Pool 的唯一所有权生命周期
        FixedBlockPool(const FixedBlockPool &) = delete;
        FixedBlockPool &operator=(const FixedBlockPool &) = delete;
        FixedBlockPool(FixedBlockPool &&) noexcept = delete;
        FixedBlockPool &operator=(FixedBlockPool &&) noexcept = delete;

        MemoryBlock Allocate() override;

        bool Deallocate(MemoryBlock block) override;

        [[nodiscard]]
        bool Owns(MemoryBlock block) const noexcept override;

        [[nodiscard]]
        std::size_t BlockSize() const noexcept override;

        [[nodiscard]]
        std::size_t Capacity() const noexcept override;

        [[nodiscard]]
        std::size_t AvailableBlocks() const noexcept override;

    private:
        // 🔒 V1 工业级内部实现架构完全冻结
        std::unique_ptr<std::byte[]> m_storage; // RAII 资源持有：托管整块池内存
        std::vector<std::size_t> m_freeList;    // 索引型 FreeList：连续内存，Cache Friendly 极佳
        std::vector<std::uint8_t> m_allocated;  // 🛡️ 状态标记数组：彻底干掉 std::vector<bool>，无双重所有权污染，零调试阻碍
        std::size_t m_blockSize;                // 单个块的对齐后净尺寸
        std::size_t m_blockCount;               // 池内容纳的块最大总数
    };
}