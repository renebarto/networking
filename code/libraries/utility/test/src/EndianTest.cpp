#include "GoogleTest.h"

#include "utility/Endian.h"

namespace utility {

TEST(EndianTest, PlatformEndianness)
{
    EXPECT_EQ(Endianness::LittleEndian, PlatformEndianness());
}

TEST(EndianTest, ToNetworkByteOrder)
{
    EXPECT_EQ(0x12, ToNetworkByteOrder(std::int8_t {0x12}));
    EXPECT_EQ(0x12, ToNetworkByteOrder(std::uint8_t {0x12}));
    EXPECT_EQ(0x3412, ToNetworkByteOrder(std::int16_t {0x1234}));
    EXPECT_EQ(0x3412, ToNetworkByteOrder(std::uint16_t {0x1234}));
    EXPECT_EQ(0x78563412, ToNetworkByteOrder(std::int32_t {0x12345678}));
    EXPECT_EQ(0x78563412u, ToNetworkByteOrder(std::uint32_t {0x12345678}));
    EXPECT_EQ(0xEFCDAB8967452301ll, ToNetworkByteOrder(std::int64_t {0x0123456789ABCDEF}));
    EXPECT_EQ(0xEFCDAB8967452301ull, ToNetworkByteOrder(std::uint64_t {0x0123456789ABCDEF}));
}

TEST(EndianTest, FromNetworkByteOrder)
{
    EXPECT_EQ(0x12, FromNetworkByteOrder(std::int8_t {0x12}));
    EXPECT_EQ(0x12, FromNetworkByteOrder(std::uint8_t {0x12}));
    EXPECT_EQ(0x3412, FromNetworkByteOrder(std::int16_t {0x1234}));
    EXPECT_EQ(0x3412, FromNetworkByteOrder(std::uint16_t {0x1234}));
    EXPECT_EQ(0x78563412, FromNetworkByteOrder(std::int32_t {0x12345678}));
    EXPECT_EQ(0x78563412u, FromNetworkByteOrder(std::uint32_t {0x12345678}));
    EXPECT_EQ(0xEFCDAB8967452301ll, FromNetworkByteOrder(std::int64_t {0x0123456789ABCDEF}));
    EXPECT_EQ(0xEFCDAB8967452301ull, FromNetworkByteOrder(std::uint64_t {0x0123456789ABCDEF}));
}

} // namespace utility
