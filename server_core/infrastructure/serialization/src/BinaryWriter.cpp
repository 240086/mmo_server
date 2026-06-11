#include <mmo/infrastructure/serialization/BinaryWriter.hpp>
#include <mmo/infrastructure/serialization/StringSerializer.hpp>

namespace mmo::infrastructure::serialization
{
    BinaryWriter::BinaryWriter(
        packet::PacketWriter &writer) noexcept
        : m_writer(writer)
    {
    }

    bool BinaryWriter::WriteString(
        std::string_view value)
    {
        return StringSerializer::Serialize(
            *this,
            value);
    }

    bool BinaryWriter::WriteBytes(
        const void *data,
        std::size_t size)
    {
        if (size == 0) [[unlikely]]
        {
            return true;
        }

        if (data == nullptr) [[unlikely]]
        {
            return false;
        }

        return m_writer.WriteBytes(data, size);
    }
}