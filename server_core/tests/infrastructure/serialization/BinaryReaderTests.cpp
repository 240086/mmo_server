#include <gtest/gtest.h>

#include <string>

#include <mmo/infrastructure/packet/PacketBuffer.hpp>
#include <mmo/infrastructure/packet/PacketWriter.hpp>
#include <mmo/infrastructure/packet/PacketReader.hpp>
#include <mmo/infrastructure/packet/PacketView.hpp>

#include <mmo/infrastructure/serialization/BinaryWriter.hpp>
#include <mmo/infrastructure/serialization/BinaryReader.hpp>

using namespace mmo::infrastructure;

namespace
{
    struct ReaderFixture
    {
        packet::PacketBuffer buffer{1024};

        packet::PacketWriter packetWriter{buffer};

        serialization::BinaryWriter writer{packetWriter};

        packet::PacketReader CreateReader()
        {
            packet::PacketView view(
                buffer.Data(),
                packetWriter.GetOffset());

            return packet::PacketReader(view);
        }
    };
}

TEST(BinaryReaderTests, ReadPrimitive)
{
    ReaderFixture fixture;

    ASSERT_TRUE(
        fixture.writer.Write(uint32_t{777}));

    auto readerImpl =
        fixture.CreateReader();

    serialization::BinaryReader reader(
        readerImpl);

    uint32_t value{0};

    ASSERT_TRUE(
        reader.Read(value));

    EXPECT_EQ(
        value,
        777u);
}

TEST(BinaryReaderTests, ReadString)
{
    ReaderFixture fixture;

    ASSERT_TRUE(
        fixture.writer.WriteString(
            "MMO"));

    auto readerImpl =
        fixture.CreateReader();

    serialization::BinaryReader reader(
        readerImpl);

    std::string result;

    ASSERT_TRUE(
        reader.ReadString(result));

    EXPECT_EQ(
        result,
        "MMO");
}

TEST(BinaryReaderTests, ReadBytesRejectsNullDestination)
{
    ReaderFixture fixture;

    auto readerImpl =
        fixture.CreateReader();

    serialization::BinaryReader reader(
        readerImpl);

    EXPECT_FALSE(
        reader.ReadBytes(
            nullptr,
            8));
}

TEST(BinaryReaderTests, ReadBytesAcceptsZeroLength)
{
    ReaderFixture fixture;

    auto readerImpl =
        fixture.CreateReader();

    serialization::BinaryReader reader(
        readerImpl);

    EXPECT_TRUE(
        reader.ReadBytes(
            nullptr,
            0));
}

TEST(BinaryReaderTests, MultipleReadsRemainSequential)
{
    ReaderFixture fixture;

    ASSERT_TRUE(
        fixture.writer.Write(uint32_t{10}));

    ASSERT_TRUE(
        fixture.writer.Write(uint64_t{20}));

    ASSERT_TRUE(
        fixture.writer.Write(uint16_t{30}));

    auto readerImpl =
        fixture.CreateReader();

    serialization::BinaryReader reader(
        readerImpl);

    uint32_t a{};
    uint64_t b{};
    uint16_t c{};

    ASSERT_TRUE(reader.Read(a));
    ASSERT_TRUE(reader.Read(b));
    ASSERT_TRUE(reader.Read(c));

    EXPECT_EQ(a, 10u);
    EXPECT_EQ(b, 20u);
    EXPECT_EQ(c, 30u);
}

TEST(BinaryReaderTests, ReadFailsWhenDataExhausted)
{
    ReaderFixture fixture;

    ASSERT_TRUE(
        fixture.writer.Write(uint32_t{1}));

    auto readerImpl =
        fixture.CreateReader();

    serialization::BinaryReader reader(
        readerImpl);

    uint32_t value{};

    ASSERT_TRUE(
        reader.Read(value));

    EXPECT_FALSE(
        reader.Read(value));
}