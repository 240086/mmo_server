// /home/xj/projects/mmo_server/server_core/infrastructure/packet/include/mmo/infrastructure/packet/PacketReader.hpp
#pragma once

#include <cstddef>
#include <type_traits>
#include <cstring>
#include <string>
#include <mmo/infrastructure/packet/PacketView.hpp>

namespace mmo::infrastructure::packet
{
    class PacketReader
    {
    public:
        explicit PacketReader(PacketView view) noexcept;

        // 安全边界检测模板读取器
        template <typename T>
            requires std::is_trivially_copyable_v<T>
        [[nodiscard]] bool Read(T &value)
        {
            return ReadBytes(&value, sizeof(T));
        }

        [[nodiscard]]
        bool ReadBytes(void *dst, std::size_t size);

        [[nodiscard]]
        bool ReadString(std::string &str);

        [[nodiscard]]
        std::size_t GetRemainingBytes() const noexcept;

        [[nodiscard]]
        std::size_t GetOffset() const noexcept;

    private:
        PacketView m_view;
        std::size_t m_offset;
    };
}