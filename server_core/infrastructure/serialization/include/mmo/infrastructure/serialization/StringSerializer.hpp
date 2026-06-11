// server_core/infrastructure/serialization/include/mmo/infrastructure/serialization/StringSerializer.hpp
#pragma once

#include <string>
#include <string_view>

namespace mmo::infrastructure::serialization
{
    class BinaryWriter;
    class BinaryReader;

    struct StringSerializer
    {
        // ⚡ 完美的 string_view 传参，消灭运行期的二次零时拷贝
        static bool Serialize(BinaryWriter &writer, std::string_view value);

        static bool Deserialize(BinaryReader &reader, std::string &value);
    };
}