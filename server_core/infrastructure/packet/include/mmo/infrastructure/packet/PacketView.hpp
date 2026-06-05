// /home/xj/projects/mmo_server/server_core/infrastructure/packet/include/mmo/infrastructure/packet/PacketView.hpp
#pragma once

#include <cstddef>

namespace mmo::infrastructure::packet
{
    // ⚡ 架构师设计：非占有性借用结构（类似于 std::span），禁止任何形式的堆内存分配
    class PacketView
    {
    public:
        PacketView(const std::byte *data, std::size_t size) noexcept;

        [[nodiscard]] const std::byte *Data() const noexcept;
        [[nodiscard]] std::size_t Size() const noexcept;

        [[nodiscard]] bool Empty() const noexcept;
        [[nodiscard]] PacketView SubView(std::size_t offset, std::size_t size) const;

    private:
        const std::byte *m_data;
        std::size_t m_size;
    };
}