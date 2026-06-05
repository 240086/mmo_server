// /home/xj/projects/mmo_server/server_core/infrastructure/packet/include/mmo/infrastructure/packet/PacketWriter.hpp
#pragma once

#include <cstddef>
#include <type_traits>
#include <string_view>
#include <cstring>
#include <mmo/infrastructure/packet/PacketBuffer.hpp>

namespace mmo::infrastructure::packet
{
    class PacketWriter
    {
    public:
        explicit PacketWriter(PacketBuffer &buffer) noexcept;

        // 🧠 数据结构工程师硬核护航：使用 C++ 概念机制强行限制输入类型，严禁虚指针和非连续内存对象混入
        template <typename T>
            requires std::is_trivially_copyable_v<T>
        [[nodiscard]] bool Write(const T &value)
        {
            return WriteBytes(&value, sizeof(T));
        }

        [[nodiscard]] bool WriteBytes(const void *data, std::size_t size);

        // 未来：
        // Serialization
        // RPC
        // Protocol

        [[nodiscard]] bool WriteString(std::string_view str);

        [[nodiscard]] bool WriteSpan(const std::byte *data, std::size_t size);

        [[nodiscard]] std::size_t GetOffset() const noexcept;
        void Reset() noexcept;

    private:
        PacketBuffer &m_buffer;
        std::size_t m_offset;
    };
}