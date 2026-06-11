// server_core/infrastructure/serialization/src/StringSerializer.cpp
#include <string>
#include <mmo/infrastructure/serialization/StringSerializer.hpp>
#include <mmo/infrastructure/serialization/BinaryReader.hpp>
#include <mmo/infrastructure/serialization/BinaryWriter.hpp>
#include <mmo/infrastructure/serialization/SerializationConstants.hpp>
#include <mmo/infrastructure/serialization/SerializationTypes.hpp>

namespace mmo::infrastructure::serialization
{
    bool StringSerializer::Serialize(BinaryWriter &writer, std::string_view value)
    {
        // 🛡️ 架构师安全锁：严防上游超大 size_t 在 static_cast 时发生高位截断
        if (value.size() > MaxStringLength)
        {
            return false;
        }

        const auto length = static_cast<StringLengthType>(value.size());

        if (!writer.Write(length))
        {
            return false;
        }

        if (length == 0)
        {
            return true;
        }

        return writer.WriteBytes(value.data(), length);
    }

    bool StringSerializer::Deserialize(BinaryReader &reader, std::string &value)
    {
        StringLengthType length{0};

        if (!reader.Read(length))
        {
            return false;
        }

        // 🛡️ 防御性边界：拦截恶意的 Oversized 长度轰炸，御敌于千里之外
        if (length > MaxStringLength)
        {
            return false;
        }

        if (length == 0)
        {
            value.clear();
            return true;
        }

        // ⚡ 性能优化：原地复用既有缓存，拒绝 temp 变量引发的二次堆内存分配
        value.resize(length);

        if (!reader.ReadBytes(value.data(), length))
        {
            // 🔒 状态防腐：一旦底层字节流残缺，立即抹平状态，拒绝“半初始化字符串”外泄
            value.clear();
            return false;
        }

        return true;
    }
}