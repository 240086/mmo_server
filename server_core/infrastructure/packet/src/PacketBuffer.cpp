// server_core/infrastructure/packet/src/PacketBuffer.cpp

#include <stdexcept>

#include <mmo/infrastructure/packet/PacketBuffer.hpp>

namespace mmo::infrastructure::packet
{
    namespace
    {
        [[nodiscard]]
        bool IsValidCapacity(std::size_t size) noexcept
        {
            return size <= kMaxPacketSize;
        }
    }

    PacketBuffer::PacketBuffer(std::size_t capacity)
    {
        if (!IsValidCapacity(capacity))
        {
            throw std::length_error(
                "PacketBuffer capacity exceeds kMaxPacketSize.");
        }

        m_buffer.reserve(capacity);
    }

    std::byte *PacketBuffer::Data() noexcept
    {
        return m_buffer.data();
    }

    const std::byte *PacketBuffer::Data() const noexcept
    {
        return m_buffer.data();
    }

    std::size_t PacketBuffer::Size() const noexcept
    {
        return m_buffer.size();
    }

    std::size_t PacketBuffer::Capacity() const noexcept
    {
        return m_buffer.capacity();
    }

    bool PacketBuffer::Empty() const noexcept
    {
        return m_buffer.empty();
    }

    void PacketBuffer::Resize(std::size_t size)
    {
        if (!IsValidCapacity(size))
        {
            throw std::length_error(
                "PacketBuffer resize exceeds kMaxPacketSize.");
        }

        m_buffer.resize(size);
    }

    void PacketBuffer::Reserve(std::size_t capacity)
    {
        if (!IsValidCapacity(capacity))
        {
            throw std::length_error(
                "PacketBuffer reserve exceeds kMaxPacketSize.");
        }

        m_buffer.reserve(capacity);
    }

    void PacketBuffer::Clear() noexcept
    {
        m_buffer.clear();
    }
}