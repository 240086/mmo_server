// server_core/infrastructure/serialization/include/mmo/infrastructure/serialization/SerializationTraits.hpp
#pragma once

#include <type_traits>

namespace mmo::infrastructure::serialization
{
    template <typename T>
    struct SerializationTraits
    {
        // 🛡️ 架构师安全锁：支持 Trivially Copyable，但死死卡住原生指针与 nullptr，防止内存地址外泄
        static constexpr bool Supported =
            std::is_trivially_copyable_v<T> &&
            !std::is_pointer_v<T> &&
            !std::is_reference_v<T> &&
            !std::is_null_pointer_v<T>;
    };

    template <typename T>
    inline constexpr bool IsSerializableV = SerializationTraits<T>::Supported;

    // 🔒 物理冷冻：如果当前宿主机不是小端架构，编译期直接掀桌子
    static_assert(std::endian::native == std::endian::little,
                  "Fatal: mmo::serialization V1 only supports Little-Endian platforms!");
}