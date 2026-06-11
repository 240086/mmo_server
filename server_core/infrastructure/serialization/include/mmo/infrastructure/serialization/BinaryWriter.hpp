// server_core/infrastructure/serialization/include/mmo/infrastructure/serialization/BinaryWriter.hpp
#pragma once

#include <string_view>
#include <cstddef>
#include <mmo/infrastructure/packet/PacketWriter.hpp>
#include <mmo/infrastructure/serialization/PrimitiveSerializer.hpp>

namespace mmo::infrastructure::serialization
{
    class BinaryWriter
    {
    public:
        explicit BinaryWriter(packet::PacketWriter &writer) noexcept;

        ~BinaryWriter() = default;

        // 禁止拷贝，维持生命周期唯一性
        BinaryWriter(const BinaryWriter &) = delete;
        BinaryWriter &operator=(const BinaryWriter &) = delete;

        // 🚀 高层业务层直接调用的泛型点火接口
        template <typename T>
        bool Write(const T &value)
        {
            return PrimitiveSerializer::Serialize(*this, value);
        }

        bool WriteString(std::string_view value);

        // 🔗 核心桥梁接口：供下层 Serializer 写入裸流，维持单向依赖
        bool WriteBytes(const void *data, std::size_t size);

    private:
        packet::PacketWriter &m_writer;
    };

    // =================================================================
    // 延后内联实现：优雅解决 PrimitiveSerializer 与 BinaryWriter 的编译依赖环
    // =================================================================
    template <typename T>
        requires IsSerializableV<T>
    inline bool PrimitiveSerializer::Serialize(BinaryWriter &writer, const T &value)
    {
        // V1 采用极速平铺内存拷贝。因 MMO 集群环境（x86_64/ARM64 现代 Linux）皆为 Little-Endian，天然契合
        return writer.WriteBytes(&value, sizeof(T));
    }
}