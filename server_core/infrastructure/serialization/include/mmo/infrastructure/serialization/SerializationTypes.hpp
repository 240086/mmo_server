// /home/xj/projects/mmo_server/server_core/infrastructure/serialization/include/mmo/infrastructure/serialization/SerializationTypes.hpp
#pragma once

#include <cstdint>

namespace mmo::infrastructure::serialization
{
    // 🔒 显式冻结 V1 字符串长度协议，规避未来潜在的 uint16/uint64 类型混入
    using StringLengthType = std::uint32_t;
}