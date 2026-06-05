// /home/xj/projects/mmo_server/server_core/infrastructure/memory_pool/include/mmo/infrastructure/memory_pool/MemoryBlock.hpp
#pragma once

#include <cstddef>

namespace mmo::infrastructure::memory_pool
{
    struct MemoryBlock
    {
        void *data{nullptr};
        std::size_t size{0};

        [[nodiscard]]
        bool IsValid() const noexcept
        {
            return data != nullptr;
        }

        // 便捷清空方法
        void Reset() noexcept
        {
            data = nullptr;
            size = 0;
        }
    }; // 刚好 16 字节，完美命中两枚 CPU 寄存器，全速传递！
}