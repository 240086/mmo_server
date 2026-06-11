// /home/xj/projects/mmo_server/server_core/tests/infrastructure/serialization/PrimitiveSerializationTests.cpp
#include <gtest/gtest.h>
#include <cstdint>

#include <mmo/infrastructure/packet/PacketBuffer.hpp>
#include <mmo/infrastructure/packet/PacketWriter.hpp>
#include <mmo/infrastructure/packet/PacketReader.hpp>
#include <mmo/infrastructure/packet/PacketView.hpp>

#include <mmo/infrastructure/serialization/BinaryWriter.hpp>
#include <mmo/infrastructure/serialization/BinaryReader.hpp>

namespace
{
    using namespace mmo::infrastructure::packet;
    using namespace mmo::infrastructure::serialization;

    // 🚀 经典数据结构与算法工程师泛型断言模板
    template <typename T>
    void VerifyPrimitiveRoundTrip(const T value)
    {
        PacketBuffer buffer(256);

        PacketWriter packetWriter(buffer);
        BinaryWriter writer(packetWriter);

        // 💡 隐式分发给 PrimitiveSerializer 模板函数
        ASSERT_TRUE(writer.Write(value));

        PacketView view(buffer.Data(), packetWriter.GetOffset());

        PacketReader packetReader(view);
        BinaryReader reader(packetReader);

        T result{};
        ASSERT_TRUE(reader.Read(result));

        // 💡 验证值闭环复原
        EXPECT_EQ(result, value);
    }
}

// =================================================================
// 标准原生类型全量覆盖测试矩阵
// =================================================================

TEST(PrimitiveSerializationTests, Uint8RoundTrip) { VerifyPrimitiveRoundTrip<std::uint8_t>(0xFEu); }
TEST(PrimitiveSerializationTests, Uint16RoundTrip) { VerifyPrimitiveRoundTrip<std::uint16_t>(0xFEEDu); }
TEST(PrimitiveSerializationTests, Uint32RoundTrip) { VerifyPrimitiveRoundTrip<std::uint32_t>(123456789u); }
TEST(PrimitiveSerializationTests, Uint64RoundTrip) { VerifyPrimitiveRoundTrip<std::uint64_t>(0xDEADBEEFCAFEBABEull); }

TEST(PrimitiveSerializationTests, Int8RoundTrip) { VerifyPrimitiveRoundTrip<std::int8_t>(-12); }
TEST(PrimitiveSerializationTests, Int16RoundTrip) { VerifyPrimitiveRoundTrip<std::int16_t>(-4321); }
TEST(PrimitiveSerializationTests, Int32RoundTrip) { VerifyPrimitiveRoundTrip<std::int32_t>(-987654321); }
TEST(PrimitiveSerializationTests, Int64RoundTrip) { VerifyPrimitiveRoundTrip<std::int64_t>(-112233445566778899ll); }

TEST(PrimitiveSerializationTests, FloatRoundTrip) { VerifyPrimitiveRoundTrip<float>(3.1415926f); }
TEST(PrimitiveSerializationTests, DoubleRoundTrip) { VerifyPrimitiveRoundTrip<double>(2.718281828459045); }

TEST(PrimitiveSerializationTests, BoolTrueRoundTrip) { VerifyPrimitiveRoundTrip<bool>(true); }
TEST(PrimitiveSerializationTests, BoolFalseRoundTrip) { VerifyPrimitiveRoundTrip<bool>(false); }