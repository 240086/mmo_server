// /home/xj/projects/mmo_server/server_core/infrastructure/packet/src/PacketWriter.cpp
#include <mmo/infrastructure/packet/PacketWriter.hpp>

#include <cstring>
#include <cstdint>

#include <mmo/infrastructure/packet/PacketConstants.hpp>

namespace mmo::infrastructure::packet
{
    PacketWriter::PacketWriter(PacketBuffer &buffer) noexcept
        : m_buffer(buffer), m_offset(0)
    {
    }

    bool PacketWriter::WriteBytes(
        const void *data,
        std::size_t size)
    {
        if (size == 0)
        {
            return true;
        }

        if (data == nullptr)
        {
            return false;
        }

        const std::size_t requiredSize =
            m_offset + size;

        if (requiredSize > kMaxPacketSize)
        {
            return false;
        }

        if (requiredSize > m_buffer.Size())
        {
            m_buffer.Resize(requiredSize);
        }

        std::memcpy(
            m_buffer.Data() + m_offset,
            data,
            size);

        m_offset += size;

        return true;
    }

    bool PacketWriter::WriteString(
        std::string_view str)
    {
        const auto length =
            static_cast<std::uint32_t>(
                str.size());

        const std::size_t originalOffset =
            m_offset;

        if (!Write(length))
        {
            return false;
        }

        if (!WriteBytes(
                str.data(),
                str.size()))
        {
            m_offset = originalOffset;
            return false;
        }

        return true;
    }

    bool PacketWriter::WriteSpan(
        const std::byte *data,
        std::size_t size)
    {
        return WriteBytes(data, size);
    }

    std::size_t PacketWriter::GetOffset() const noexcept
    {
        return m_offset;
    }

    void PacketWriter::Reset() noexcept
    {
        m_offset = 0;
    }
}