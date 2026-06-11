// /home/xj/projects/mmo_server/server_core/tests/infrastructure/serialization/StringSerializationTests.cpp
#include <gtest/gtest.h>
#include <string>
#include <cstring>

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

    class StringSerializationTests : public ::testing::Test
    {
    protected:
        static std::string RoundTrip(const std::string &input)
        {
            PacketBuffer buffer(16 * 1024); // 放大 Buffer 容纳 Long String

            PacketWriter packetWriter(buffer);
            BinaryWriter writer(packetWriter);

            EXPECT_TRUE(writer.WriteString(input));

            PacketView view(buffer.Data(), packetWriter.GetOffset());

            PacketReader packetReader(view);
            BinaryReader reader(packetReader);

            std::string output;
            EXPECT_TRUE(reader.ReadString(output));

            return output;
        }
    };
}

// =================================================================
// 1. 空字符串边缘契约测试
// =================================================================
TEST_F(StringSerializationTests, EmptyStringRoundTrip)
{
    const std::string input;
    const auto output = RoundTrip(input);
    EXPECT_EQ(output, input);
}

// =================================================================
// 2. 标准 ASCII 字符串测试
// =================================================================
TEST_F(StringSerializationTests, AsciiStringRoundTrip)
{
    const std::string input = "Hello MMO";
    const auto output = RoundTrip(input);
    EXPECT_EQ(output, input);
}

// =================================================================
// 3. UTF8 多字节编码测试（🛡️ 解决 C++20 char8_t 编译硬伤）
// =================================================================
TEST_F(StringSerializationTests, Utf8StringRoundTrip)
{
    // 💡 架构师修正：通过 reinterpret_cast 将 C++20 的 char8_t[] 强制降维回 char*，喂给 std::string
    const std::string input(reinterpret_cast<const char *>(u8"你好 MMO 世界"));

    const auto output = RoundTrip(input);
    EXPECT_EQ(output, input);
}

// =================================================================
// 4. 大负载长字符串测试
// =================================================================
TEST_F(StringSerializationTests, LongStringRoundTrip)
{
    const std::string input(8192, 'A');
    const auto output = RoundTrip(input);
    EXPECT_EQ(output, input);
}

// =================================================================
// 5. 验证流的 Append-Only 追加契约（无互相污染）
// =================================================================
TEST_F(StringSerializationTests, MultipleStringsRoundTrip)
{
    PacketBuffer buffer(4096);

    PacketWriter packetWriter(buffer);
    BinaryWriter writer(packetWriter);

    ASSERT_TRUE(writer.WriteString("Alpha"));
    ASSERT_TRUE(writer.WriteString("Beta"));
    ASSERT_TRUE(writer.WriteString("Gamma"));

    PacketView view(buffer.Data(), packetWriter.GetOffset());

    PacketReader packetReader(view);
    BinaryReader reader(packetReader);

    std::string a, b, c;

    EXPECT_TRUE(reader.ReadString(a));
    EXPECT_TRUE(reader.ReadString(b));
    EXPECT_TRUE(reader.ReadString(c));

    EXPECT_EQ(a, "Alpha");
    EXPECT_EQ(b, "Beta");
    EXPECT_EQ(c, "Gamma");
}

// =================================================================
// 6. 确定性序列化测试（内存状态级确定性断言）
// =================================================================
TEST_F(StringSerializationTests, SerializationIsDeterministic)
{
    PacketBuffer lhs(1024);
    PacketBuffer rhs(1024);

    PacketWriter writerA(lhs);
    PacketWriter writerB(rhs);

    BinaryWriter binaryA(writerA);
    BinaryWriter binaryB(writerB);

    ASSERT_TRUE(binaryA.WriteString("Deterministic"));
    ASSERT_TRUE(binaryB.WriteString("Deterministic"));

    ASSERT_EQ(writerA.GetOffset(), writerB.GetOffset());

    // 💡 核心断言：完全相同的输入，其二进制内存布局必须 100% 达成无差别的绝对一致
    EXPECT_EQ(std::memcmp(lhs.Data(), rhs.Data(), writerA.GetOffset()), 0);
}