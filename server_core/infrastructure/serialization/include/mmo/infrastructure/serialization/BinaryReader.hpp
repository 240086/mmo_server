// server_core/infrastructure/serialization/include/mmo/infrastructure/serialization/BinaryReader.hpp
#pragma once

#include <string>
#include <cstddef>
#include <mmo/infrastructure/packet/PacketReader.hpp>
#include <mmo/infrastructure/serialization/PrimitiveSerializer.hpp>

namespace mmo::infrastructure::serialization
{
    class BinaryReader
    {
    public:
        explicit BinaryReader(packet::PacketReader &reader) noexcept;

        ~BinaryReader() = default;

        BinaryReader(const BinaryReader &) = delete;
        BinaryReader &operator=(const BinaryReader &) = delete;

        // 🚀 高层业务层直接调用的泛型解包接口
        template <typename T>
        bool Read(T &value)
        {
            return PrimitiveSerializer::Deserialize(*this, value);
        }

        bool ReadString(std::string &value);

        // 🔗 核心桥梁接口：供下层 Serializer 读取裸流
        bool ReadBytes(void *dest, std::size_t size);

    private:
        packet::PacketReader &m_reader;
    };

    // =================================================================
    // 延后内联实现
    // =================================================================
    template <typename T>
        requires IsSerializableV<T>
    inline bool PrimitiveSerializer::Deserialize(BinaryReader &reader, T &value)
    {
        return reader.ReadBytes(&value, sizeof(T));
    }
}