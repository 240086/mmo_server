#include <mmo/infrastructure/serialization/BinaryReader.hpp>
#include <mmo/infrastructure/serialization/StringSerializer.hpp>

namespace mmo::infrastructure::serialization
{
    BinaryReader::BinaryReader(
        packet::PacketReader &reader) noexcept
        : m_reader(reader)
    {
    }

    bool BinaryReader::ReadString(
        std::string &value)
    {
        return StringSerializer::Deserialize(
            *this,
            value);
    }

    bool BinaryReader::ReadBytes(
        void *dest,
        std::size_t size)
    {
        if (size == 0)
        {
            return true;
        }

        if (dest == nullptr)
        {
            return false;
        }

        return m_reader.ReadBytes(
            dest,
            size);
    }
}