// server_core/infrastructure/packet/src/PacketView.cpp

#include <mmo/infrastructure/packet/PacketView.hpp>

namespace mmo::infrastructure::packet
{
    PacketView::PacketView(
        const std::byte *data,
        std::size_t size) noexcept
        : m_data(data), m_size(size)
    {
    }

    const std::byte *PacketView::Data() const noexcept
    {
        return m_data;
    }

    std::size_t PacketView::Size() const noexcept
    {
        return m_size;
    }

    bool PacketView::Empty() const noexcept
    {
        return m_size == 0;
    }

    PacketView PacketView::SubView(
        std::size_t offset,
        std::size_t size) const
    {
        if (offset > m_size)
        {
            return PacketView(nullptr, 0);
        }

        const std::size_t remaining =
            m_size - offset;

        if (size > remaining)
        {
            return PacketView(nullptr, 0);
        }

        return PacketView(
            m_data + offset,
            size);
    }
}