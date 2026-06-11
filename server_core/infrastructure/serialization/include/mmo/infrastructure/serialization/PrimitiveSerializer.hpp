// server_core/infrastructure/serialization/include/mmo/infrastructure/serialization/PrimitiveSerializer.hpp
#pragma once

#include <mmo/infrastructure/serialization/SerializationTraits.hpp>

namespace mmo::infrastructure::serialization
{
    class BinaryWriter;
    class BinaryReader;

    struct PrimitiveSerializer
    {
        template <typename T>
            requires IsSerializableV<T>
        static bool Serialize(BinaryWriter &writer, const T &value);

        template <typename T>
            requires IsSerializableV<T>
        static bool Deserialize(BinaryReader &reader, T &value);
    };
}