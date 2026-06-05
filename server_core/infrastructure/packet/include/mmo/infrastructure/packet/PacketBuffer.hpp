// /home/xj/projects/mmo_server/server_core/infrastructure/packet/include/mmo/infrastructure/packet/PacketBuffer.hpp
#pragma once

#include <vector>
#include <cstddef>
#include <mmo/infrastructure/packet/PacketConstants.hpp>

namespace mmo::infrastructure::packet
{
    class PacketBuffer
    {
    public:
        explicit PacketBuffer(std::size_t capacity = kDefaultPacketCapacity);

        ~PacketBuffer() = default;
        PacketBuffer(const PacketBuffer &) = default;
        PacketBuffer &operator=(const PacketBuffer &) = default;
        PacketBuffer(PacketBuffer &&) noexcept = default;
        PacketBuffer &operator=(PacketBuffer &&) noexcept = default;

        std::byte *Data() noexcept;
        const std::byte *Data() const noexcept;

        std::size_t Size() const noexcept;
        std::size_t Capacity() const noexcept;

        [[nodiscard]]
        bool Empty() const noexcept;

        void Resize(std::size_t size);
        void Reserve(std::size_t capacity);
        void Clear() noexcept;

    private:
        std::vector<std::byte> m_buffer;
    };
}