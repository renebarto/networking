//------------------------------------------------------------------------------
// Copyright   : Copyright(c) 2020, Koninklijke Philips Electronics N.V.
//
// File        : SerializeTest.cpp
//
// Namespace   : core::serialization
//
// Class       : -
//
// Description : 
//  Test for serialization
//
//------------------------------------------------------------------------------

#include "core/serialization/Serialize.h"

#include <map>
#include <string>
#include "GoogleTest.h"

namespace core {
namespace serialization {

TEST(SerializeTest, ToString_PlainOldDataTypes)
{
    EXPECT_EQ("123", ToString(int {123}));
    EXPECT_EQ("123", ToString(123U));
    EXPECT_EQ("123", ToString(123L));
    EXPECT_EQ("123", ToString(123UL));
    EXPECT_EQ("123", ToString(std::int8_t {123}));
    EXPECT_EQ("123", ToString(std::uint8_t {123}));
    EXPECT_EQ("123", ToString(std::int16_t {123}));
    EXPECT_EQ("123", ToString(std::uint16_t {123}));
    EXPECT_EQ("123", ToString(std::int32_t {123}));
    EXPECT_EQ("123", ToString(std::uint32_t {123}));
    EXPECT_EQ("123", ToString(std::int64_t {123}));
    EXPECT_EQ("123", ToString(std::uint64_t {123}));
    EXPECT_EQ("123.456", ToString(float {123.456F}).substr(0, 7));
    EXPECT_EQ("123.456", ToString(double {123.456}).substr(0, 7));
    EXPECT_EQ("123", ToString(char {123}));
    EXPECT_EQ("123", ToString('\173'));
}

TEST(SerializeTest, ToStringHex_PlainOldDataTypes)
{
    EXPECT_EQ("0x12345678", ToStringHex(int {0x12345678}));
    EXPECT_EQ("0x12345678", ToStringHex(0x12345678U));
    EXPECT_EQ("0x12345678", ToStringHex(0x12345678L));
    EXPECT_EQ("0x12345678", ToStringHex(0x12345678UL));
    EXPECT_EQ("0x00000078", ToStringHex(std::int8_t {0x78}));
    EXPECT_EQ("0x00000078", ToStringHex(std::uint8_t {0x78}));
    EXPECT_EQ("0x00005678", ToStringHex(std::int16_t {0x5678}));
    EXPECT_EQ("0x00005678", ToStringHex(std::uint16_t {0x5678}));
    EXPECT_EQ("0x12345678", ToStringHex(std::int32_t {0x12345678}));
    EXPECT_EQ("0x12345678", ToStringHex(std::uint32_t {0x12345678}));
    EXPECT_EQ("0x123456789ABCDEF0", ToStringHex(std::int64_t {0x123456789ABCDEF0}));
    EXPECT_EQ("0x123456789ABCDEF0", ToStringHex(std::uint64_t {0x123456789ABCDEF0}));
    EXPECT_EQ("0x00000078", ToStringHex(char {0x78}));
    EXPECT_EQ("0x00000078", ToStringHex('\170'));
}

TEST(SerializeTest, ToStringHexNoBase_PlainOldDataTypes)
{
    EXPECT_EQ("12345678", ToStringHexNoBase(int {0x12345678}));
    EXPECT_EQ("12345678", ToStringHexNoBase(0x12345678U));
    EXPECT_EQ("12345678", ToStringHexNoBase(0x12345678L));
    EXPECT_EQ("12345678", ToStringHexNoBase(0x12345678UL));
    EXPECT_EQ("00000078", ToStringHexNoBase(std::int8_t {0x78}));
    EXPECT_EQ("00000078", ToStringHexNoBase(std::uint8_t {0x78}));
    EXPECT_EQ("00005678", ToStringHexNoBase(std::int16_t {0x5678}));
    EXPECT_EQ("00005678", ToStringHexNoBase(std::uint16_t {0x5678}));
    EXPECT_EQ("12345678", ToStringHexNoBase(std::int32_t {0x12345678}));
    EXPECT_EQ("12345678", ToStringHexNoBase(std::uint32_t {0x12345678}));
    EXPECT_EQ("123456789ABCDEF0", ToStringHexNoBase(std::int64_t {0x123456789ABCDEF0}));
    EXPECT_EQ("123456789ABCDEF0", ToStringHexNoBase(std::uint64_t {0x123456789ABCDEF0}));
    EXPECT_EQ("00000078", ToStringHexNoBase(char {0x78}));
    EXPECT_EQ("00000078", ToStringHexNoBase('\170'));
}

TEST(SerializeTest, ToStringPtr)
{
//TICS -POR#021 We suppress warnings for Windows only
#if defined(PLATFORM_WINDOWS)
#pragma warning(disable : 4127) //TICS !POR#018 !POR#037 Expression is constant
#endif
    if (sizeof(void *) == 8)
#if defined(PLATFORM_WINDOWS)
#pragma warning(default : 4127) //TICS !POR#018 !POR#037
#endif
//TICS +POR#021
    {
        EXPECT_EQ("0x0102030405060708", ToStringPtr(reinterpret_cast<void *>(0x0102030405060708)));
    }
    else
    {
        EXPECT_EQ("0x05060708", ToStringPtr(reinterpret_cast<void *>(0x0102030405060708)));
    }
}

TEST(SerializeTest, ToString_bool)
{
    EXPECT_EQ("false", ToString(false));
    EXPECT_EQ("true", ToString(true));
}

TEST(SerializeTest, ToStringEpochTime)
{
    auto map = std::map<osal::EpochTime, std::string>
    {
        { osal::EpochTime(std::chrono::nanoseconds(0)), "1970-Jan-01 00:00:00.000000" },
        { osal::EpochTime(std::chrono::nanoseconds(1000)), "1970-Jan-01 00:00:00.000001" },
        { osal::EpochTime(std::chrono::seconds(1577836800)), "2020-Jan-01 00:00:00.000000" },
        { osal::EpochTime(std::chrono::seconds(1601555696)), "2020-Oct-01 12:34:56.000000" },
    };
    for (auto const & item : map)
    {
        EXPECT_EQ(item.second, ToString(item.first));
    }
}

osal::EpochTime TimeProviderZero() { return osal::EpochTime {0}; }

TEST(SerializeTest, ToStringClockForZeroTime)
{
    osal::Clock clock;
    clock.SetTimeProvider(TimeProviderZero);
    const std::string EpochDateTime = "1970-Jan-01 00:00:00.000000";
    EXPECT_EQ(EpochDateTime, ToString(clock));
}

} // namespace serialization
} // namespace core
