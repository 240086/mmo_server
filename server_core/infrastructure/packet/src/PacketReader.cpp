#include <mmo/infrastructure/packet/PacketReader.hpp>

#include <cstdint>

namespace mmo::infrastructure::packet
{
    PacketReader::PacketReader(PacketView view) noexcept
        : m_view(view), m_offset(0)
    {
    }

    bool PacketReader::ReadBytes(
        void *dst,
        std::size_t size)
    {
        if (dst == nullptr || size == 0) [[unlikely]]
        {
            return false;
        }

        if (size > GetRemainingBytes()) [[unlikely]]
        {
            return false;
        }

        std::memcpy(
            dst,
            m_view.Data() + m_offset,
            size);

        m_offset += size;

        return true;
    }

    bool PacketReader::ReadString(
        std::string &str)
    {
        std::uint32_t length = 0;

        const std::size_t originalOffset =
            m_offset;

        if (!Read(length))
        {
            return false;
        }

        if (length > GetRemainingBytes())
        {
            m_offset = originalOffset;
            return false;
        }

        str.assign(
            reinterpret_cast<
                const char *>(
                m_view.Data() + m_offset),
            length);

        m_offset += length;

        return true;
    }

    std::size_t PacketReader::GetRemainingBytes() const noexcept
    {
        return m_view.Size() - m_offset;
    }

    std::size_t PacketReader::GetOffset() const noexcept
    {
        return m_offset;
    }
}