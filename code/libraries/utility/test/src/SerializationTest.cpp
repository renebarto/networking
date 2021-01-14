#include "GoogleTest.h"

#include <cstring>
#include "utility/Serialization.h"

namespace serialization {

template <class T>
static std::ostream & PrintStandard(std::ostream & stream, std::string::size_type /*integralDigits*/, std::string::size_type fractionalDigits, T value)
{
    std::ostringstream valuePrint;
    valuePrint << std::setfill('0') << std::setprecision(static_cast<std::streamsize>(fractionalDigits)) << value;
    stream << valuePrint.str();
    return stream;
}

template <class T>
static std::ostream & PrintScientific(std::ostream & stream, std::string::size_type integralDigits, std::string::size_type fractionalDigits, T value)
    {
    std::ostringstream valuePrint;
    valuePrint << std::scientific << std::setfill('0') 
        << std::setw(static_cast<std::streamsize>(integralDigits + fractionalDigits + 1))
        << std::setprecision(static_cast<std::streamsize>(fractionalDigits)) << value;
    stream << valuePrint.str();
    return stream;
}

template <class T>
static std::string SerializeStandard(std::string::size_type width, T value)
{
    std::ostringstream stream;
    PrintStandard(stream, 0, width, value);
    return stream.str();
}

template <class T>
static std::string SerializeScientific(std::string::size_type integralDigits, std::string::size_type fractionalDigits, T value)
{
    std::ostringstream stream;
    PrintScientific(stream, integralDigits, fractionalDigits, value);
    return stream.str();
}

TEST(SerializationTest, SerializeBool)
{
    bool value = true;
    EXPECT_EQ("true", Serialize(value));
    value = false;
    EXPECT_EQ("   false", Serialize(value, 8));
}

TEST(SerializationTest, SerializeInt8)
{
    int8_t value = 0x55;
    EXPECT_EQ("01010101", Serialize(value, 0, 2));
    EXPECT_EQ("125", Serialize(value, 0, 8));
    EXPECT_EQ("85", Serialize(value, 0, 10));
    EXPECT_EQ("55", Serialize(value, 0, 16));
}

TEST(SerializationTest, SerializeUInt8)
{
    uint8_t value = 0xAA;
    EXPECT_EQ("10101010", Serialize(value, 0, 2));
    EXPECT_EQ("252", Serialize(value, 0, 8));
    EXPECT_EQ("170", Serialize(value, 0, 10));
    EXPECT_EQ("AA", Serialize(value, 0, 16));
}

TEST(SerializationTest, SerializeInt16)
{
    int16_t value = 0x5555;
    EXPECT_EQ("0101010101010101", Serialize(value, 0, 2));
    EXPECT_EQ("052525", Serialize(value, 0, 8));
    EXPECT_EQ("21845", Serialize(value, 0, 10));
    EXPECT_EQ("5555", Serialize(value, 0, 16));
}

TEST(SerializationTest, SerializeUInt16)
{
    uint16_t value = 0xAAAA;
    EXPECT_EQ("1010101010101010", Serialize(value, 0, 2));
    EXPECT_EQ("125252", Serialize(value, 0, 8));
    EXPECT_EQ("43690", Serialize(value, 0, 10));
    EXPECT_EQ("AAAA", Serialize(value, 0, 16));
}

TEST(SerializationTest, SerializeInt32)
{
    int32_t value = 0x55555555;
    EXPECT_EQ("01010101010101010101010101010101", Serialize(value, 0, 2));
    EXPECT_EQ("12525252525", Serialize(value, 0, 8));
    EXPECT_EQ("1431655765", Serialize(value, 0, 10));
    EXPECT_EQ("55555555", Serialize(value, 0, 16));
}

TEST(SerializationTest, SerializeUInt32)
{
    uint32_t value = 0xAAAAAAAA;
    EXPECT_EQ("10101010101010101010101010101010", Serialize(value, 0, 2));
    EXPECT_EQ("25252525252", Serialize(value, 0, 8));
    EXPECT_EQ("2863311530", Serialize(value, 0, 10));
    EXPECT_EQ("AAAAAAAA", Serialize(value, 0, 16));
}

TEST(SerializationTest, SerializeInt64)
{
    int64_t value = 0x5555555555555555;
    EXPECT_EQ("0101010101010101010101010101010101010101010101010101010101010101", Serialize(value, 0, 2));
    EXPECT_EQ("0525252525252525252525", Serialize(value, 0, 8));
    EXPECT_EQ("6148914691236517205", Serialize(value, 0, 10));
    EXPECT_EQ("5555555555555555", Serialize(value, 0, 16));
}

TEST(SerializationTest, SerializeUInt64)
{
    uint64_t value = 0xAAAAAAAAAAAAAAAA;
    EXPECT_EQ("1010101010101010101010101010101010101010101010101010101010101010", Serialize(value, 0, 2));
    EXPECT_EQ("1252525252525252525252", Serialize(value, 0, 8));
    EXPECT_EQ("12297829382473034410", Serialize(value, 0, 10));
    EXPECT_EQ("AAAAAAAAAAAAAAAA", Serialize(value, 0, 16));
}

TEST(SerializationTest, SerializeFloat)
{
    float value = 1.0002e4F;
    EXPECT_EQ("10002", Serialize(value));
    EXPECT_EQ("   10002", Serialize(value, 8));
    EXPECT_EQ("10002.0000", Serialize(value, 0, 4, FloatingPointRepresentation::Fixed));
    EXPECT_EQ("0010002.0000", Serialize(value, 12, 4, FloatingPointRepresentation::Fixed));
    EXPECT_EQ(SerializeScientific(0, 4, value), Serialize(value, 0, 4, FloatingPointRepresentation::Exponential));
    EXPECT_EQ("1.0002e+04", Serialize(value, 0, 4, FloatingPointRepresentation::Exponential));
    EXPECT_EQ("10002", Serialize(value, 0, 8, FloatingPointRepresentation::Mixed));
    value = 5.0E-37F;
    EXPECT_EQ(SerializeStandard(16, value), Serialize(value));
   EXPECT_EQ("5.000000179695649e-37", Serialize(value));
   EXPECT_EQ("5e-37", Serialize(value, 0, 1));
}

TEST(SerializationTest, SerializeDouble)
{
    double value = 1.00002E05;
    EXPECT_EQ("100002", Serialize(value));
    EXPECT_EQ("  100002", Serialize(value, 8));
    EXPECT_EQ("100002.0000", Serialize(value, 0, 4, FloatingPointRepresentation::Fixed));
    EXPECT_EQ("0100002.0000", Serialize(value, 12, 4, FloatingPointRepresentation::Fixed));
    EXPECT_EQ(SerializeScientific(0, 4, value), Serialize(value, 0, 4, FloatingPointRepresentation::Exponential));
    EXPECT_EQ("1.0000e+05", Serialize(value, 0, 4, FloatingPointRepresentation::Exponential));
    EXPECT_EQ("100002", Serialize(value, 0, 8, FloatingPointRepresentation::Mixed));
    value = 1e-200;
    EXPECT_EQ("1e-200", Serialize(value));
    value = 1.001e-200;
    EXPECT_EQ("1.001e-200", Serialize(value));
    EXPECT_EQ("1e-200", Serialize(value, 0, 2));
}

TEST(SerializationTest, SerializeStringNarrow)
{
    std::string value = "Hello";
    std::string expected = "Hello";
    EXPECT_EQ(expected, Serialize(value));
}

TEST(SerializationTest, SerializeStringNarrowQuoted)
{
    std::string value = "Hello";
    std::string expected = "\"Hello\"";
    EXPECT_EQ(expected, Serialize(value, 0, true));
}

TEST(SerializationTest, SerializeStringWide)
{
    std::wstring value = L"Hello";
    std::string expected = "Hello";
    EXPECT_EQ(expected, Serialize(value));
}

TEST(SerializationTest, SerializeStringWideQuoted)
{
    std::wstring value = L"Hello";
    std::string expected = "\"Hello\"";
    EXPECT_EQ(expected, Serialize(value, 0, true));
}

TEST(SerializationTest, SerializeCharPtrNarrow)
{
    const char * value = "Hello";
    std::string expected = "Hello";
    EXPECT_EQ(expected, Serialize(value));
}

TEST(SerializationTest, SerializeCharPtrNarrowQuoted)
{
    const char * value = "Hello";
    std::string expected = "\"Hello\"";
    EXPECT_EQ(expected, Serialize(value, 0, true));
}

TEST(SerializationTest, SerializeCharPtrWide)
{
    const wchar_t * value = L"Hello";
    std::string expected = "Hello";
    EXPECT_EQ(expected, Serialize(value));
}

TEST(SerializationTest, SerializeCharPtrWideQuoted)
{
    const wchar_t * value = L"Hello";
    std::string expected = "\"Hello\"";
    EXPECT_EQ(expected, Serialize(value, 0, true));
}

TEST(SerializationTest, SerializeVoidPtrNull)
{
    const void * value = nullptr;
    std::string expected = "null";
    EXPECT_EQ(expected, Serialize(value, 0));
}

TEST(SerializationTest, SerializeVoidPtr)
{
    const void * value = reinterpret_cast<const void *>(0x123456789ABCDEF0llu);
    std::string expected;
    if (sizeof(value) == 4)
    {
        expected = "0x9ABCDEF0";
    }
    else
    {
        expected = "0x123456789ABCDEF0";
    }
    EXPECT_EQ(expected, Serialize(value, 0));
}

TEST(SerializationTest, SerializeUInt8PtrNull)
{
    const std::uint8_t * value = nullptr;
    std::string expected = "null";
    EXPECT_EQ(expected, SerializeData(value, std::size_t {0}));
}

TEST(SerializationTest, SerializeUInt8PtrZeroLength)
{
    const std::uint8_t value[] { 0x00 };
    std::string expected = "";
    EXPECT_EQ(expected, SerializeData(value, std::size_t {0}));
}

TEST(SerializationTest, SerializeUInt8PtrOneByte)
{
    const std::uint8_t value[] { 0x41 };
    std::string expected = "41                                               A                              \n";
    EXPECT_EQ(expected, SerializeData(value, sizeof(value)));
}

TEST(SerializationTest, SerializeUInt8PtrFifteenBytes)
{
    const std::uint8_t value[] { 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F };
    std::string expected = "41 42 43 44 45 46 47 48 49 4A 4B 4C 4D 4E 4F     A B C D E F G H I J K L M N O  \n";
    EXPECT_EQ(expected, SerializeData(value, sizeof(value)));
}

TEST(SerializationTest, SerializeUInt8PtrSixteenBytes)
{
    const std::uint8_t value[] { 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50 };
    std::string expected = "41 42 43 44 45 46 47 48 49 4A 4B 4C 4D 4E 4F 50  A B C D E F G H I J K L M N O P\n";
    EXPECT_EQ(expected, SerializeData(value, sizeof(value)));
}

TEST(SerializationTest, SerializeUInt8PtrThirtyTwoBytesWithNonPrintables)
{
    const std::uint8_t value[] { 0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48, 0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50,
                                 0x11, 0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20 };
    std::string expected = 
        "41 42 43 44 45 46 47 48 49 4A 4B 4C 4D 4E 4F 50  A B C D E F G H I J K L M N O P\n"
        "11 12 13 14 15 16 17 18 19 1A 1B 1C 1D 1E 1F 20  . . . . . . . . . . . . . . .  \n";
    EXPECT_EQ(expected, SerializeData(value, sizeof(value)));
}

TEST(SerializationTest, SerializeBinaryBool)
{
    bool value = true;
    std::vector<std::uint8_t> buffer;
    std::size_t offset = 0;
    SerializeBinary(value, buffer, offset);
    value = false;
    SerializeBinary(value, buffer, offset);
    EXPECT_EQ(std::size_t {2}, offset);
    EXPECT_EQ(offset, buffer.size());
    EXPECT_EQ(std::vector<std::uint8_t>({ 0x01, 0x00 }), buffer);
}

TEST(SerializationTest, SerializeBinaryInt8)
{
    std::int8_t value = 123;
    std::vector<std::uint8_t> buffer;
    std::size_t offset = 0;
    SerializeBinary(value, buffer, offset);
    EXPECT_EQ(std::size_t {1}, offset);
    EXPECT_EQ(offset, buffer.size());
    EXPECT_EQ(std::vector<std::uint8_t>({ 123 }), buffer);
}

TEST(SerializationTest, SerializeBinaryUInt8)
{
    std::uint8_t value = 234;
    std::vector<std::uint8_t> buffer;
    std::size_t offset = 0;
    SerializeBinary(value, buffer, offset);
    EXPECT_EQ(std::size_t {1}, offset);
    EXPECT_EQ(offset, buffer.size());
    EXPECT_EQ(std::vector<std::uint8_t>({ 234 }), buffer);
}

TEST(SerializationTest, SerializeBinaryInt16LittleEndian)
{
    std::int16_t value = 0x1234;
    std::vector<std::uint8_t> buffer;
    std::size_t offset = 0;
    SerializeBinary(value, buffer, offset, utility::Endianness::LittleEndian);
    EXPECT_EQ(std::size_t {2}, offset);
    EXPECT_EQ(offset, buffer.size());
    EXPECT_EQ(std::vector<std::uint8_t>({ 0x34, 0x12 }), buffer);
}

TEST(SerializationTest, SerializeBinaryInt16BigEndian)
{
    std::int16_t value = 0x1234;
    std::vector<std::uint8_t> buffer;
    std::size_t offset = 0;
    SerializeBinary(value, buffer, offset, utility::Endianness::BigEndian);
    EXPECT_EQ(std::size_t {2}, offset);
    EXPECT_EQ(offset, buffer.size());
    EXPECT_EQ(std::vector<std::uint8_t>({ 0x12, 0x34 }), buffer);
}

TEST(SerializationTest, SerializeBinaryUInt16LittleEndian)
{
    std::uint16_t value = 0x8765;
    std::vector<std::uint8_t> buffer;
    std::size_t offset = 0;
    SerializeBinary(value, buffer, offset, utility::Endianness::LittleEndian);
    EXPECT_EQ(std::size_t {2}, offset);
    EXPECT_EQ(offset, buffer.size());
    EXPECT_EQ(std::vector<std::uint8_t>({ 0x65, 0x87 }), buffer);
}

TEST(SerializationTest, SerializeBinaryUInt16BigEndian)
{
    std::uint16_t value = 0x8765;
    std::vector<std::uint8_t> buffer;
    std::size_t offset = 0;
    SerializeBinary(value, buffer, offset, utility::Endianness::BigEndian);
    EXPECT_EQ(std::size_t {2}, offset);
    EXPECT_EQ(offset, buffer.size());
    EXPECT_EQ(std::vector<std::uint8_t>({ 0x87, 0x65 }), buffer);
}

TEST(SerializationTest, SerializeBinaryInt32LittleEndian)
{
    std::int32_t value = 0x12345678;
    std::vector<std::uint8_t> buffer;
    std::size_t offset = 0;
    SerializeBinary(value, buffer, offset, utility::Endianness::LittleEndian);
    EXPECT_EQ(std::size_t {4}, offset);
    EXPECT_EQ(offset, buffer.size());
    EXPECT_EQ(std::vector<std::uint8_t>({ 0x78, 0x56, 0x34, 0x12 }), buffer);
}

TEST(SerializationTest, SerializeBinaryInt32BigEndian)
{
    std::int32_t value = 0x12345678;
    std::vector<std::uint8_t> buffer;
    std::size_t offset = 0;
    SerializeBinary(value, buffer, offset, utility::Endianness::BigEndian);
    EXPECT_EQ(std::size_t {4}, offset);
    EXPECT_EQ(offset, buffer.size());
    EXPECT_EQ(std::vector<std::uint8_t>({ 0x12, 0x34, 0x56, 0x78 }), buffer);
}

TEST(SerializationTest, SerializeBinaryUInt32LittleEndian)
{
    std::uint32_t value = 0x87654321;
    std::vector<std::uint8_t> buffer;
    std::size_t offset = 0;
    SerializeBinary(value, buffer, offset, utility::Endianness::LittleEndian);
    EXPECT_EQ(std::size_t {4}, offset);
    EXPECT_EQ(offset, buffer.size());
    EXPECT_EQ(std::vector<std::uint8_t>({ 0x21, 0x43, 0x65, 0x87 }), buffer);
}

TEST(SerializationTest, SerializeBinaryUInt32BigEndian)
{
    std::uint32_t value = 0x87654321;
    std::vector<std::uint8_t> buffer;
    std::size_t offset = 0;
    SerializeBinary(value, buffer, offset, utility::Endianness::BigEndian);
    EXPECT_EQ(std::size_t {4}, offset);
    EXPECT_EQ(offset, buffer.size());
    EXPECT_EQ(std::vector<std::uint8_t>({ 0x87, 0x65, 0x43, 0x21 }), buffer);
}

TEST(SerializationTest, SerializeBinaryInt64LittleEndian)
{
    std::int64_t value = 0x0123456789ABCDEF;
    std::vector<std::uint8_t> buffer;
    std::size_t offset = 0;
    SerializeBinary(value, buffer, offset, utility::Endianness::LittleEndian);
    EXPECT_EQ(std::size_t {8}, offset);
    EXPECT_EQ(offset, buffer.size());
    EXPECT_EQ(std::vector<std::uint8_t>({ 0xEF, 0xCD, 0xAB, 0x89, 0x67, 0x45, 0x23, 0x01 }), buffer);
}

TEST(SerializationTest, SerializeBinaryInt64BigEndian)
{
    std::int64_t value = 0x0123456789ABCDEF;
    std::vector<std::uint8_t> buffer;
    std::size_t offset = 0;
    SerializeBinary(value, buffer, offset, utility::Endianness::BigEndian);
    EXPECT_EQ(std::size_t {8}, offset);
    EXPECT_EQ(offset, buffer.size());
    EXPECT_EQ(std::vector<std::uint8_t>({ 0x01, 0x23, 0x45, 0x67, 0x89, 0xAB, 0xCD, 0xEF }), buffer);
}

TEST(SerializationTest, SerializeBinaryUInt64LittleEndian)
{
    std::uint64_t value = 0xFEDCBA9876543210;
    std::vector<std::uint8_t> buffer;
    std::size_t offset = 0;
    SerializeBinary(value, buffer, offset, utility::Endianness::LittleEndian);
    EXPECT_EQ(std::size_t {8}, offset);
    EXPECT_EQ(offset, buffer.size());
    EXPECT_EQ(std::vector<std::uint8_t>({ 0x10, 0x32, 0x54, 0x76, 0x98, 0xBA, 0xDC, 0xFE }), buffer);
}

TEST(SerializationTest, SerializeBinaryUInt64BigEndian)
{
    std::uint64_t value = 0xFEDCBA9876543210;
    std::vector<std::uint8_t> buffer;
    std::size_t offset = 0;
    SerializeBinary(value, buffer, offset, utility::Endianness::BigEndian);
    EXPECT_EQ(std::size_t {8}, offset);
    EXPECT_EQ(offset, buffer.size());
    EXPECT_EQ(std::vector<std::uint8_t>({ 0xFE, 0xDC, 0xBA, 0x98, 0x76, 0x54, 0x32, 0x10 }), buffer);
}

TEST(SerializationTest, SerializeBinaryFloatLittleEndian)
{
    float value = 0.5F;
    std::vector<std::uint8_t> buffer;
    std::size_t offset = 0;
    SerializeBinary(value, buffer, offset, utility::Endianness::LittleEndian);
    EXPECT_EQ(std::size_t {4}, offset);
    EXPECT_EQ(offset, buffer.size());
    EXPECT_EQ(std::vector<std::uint8_t>({ 0x00, 0x00, 0x00, 0x3F }), buffer);
}

TEST(SerializationTest, SerializeBinaryFloatBigEndian)
{
    float value = 0.5F;
    std::vector<std::uint8_t> buffer;
    std::size_t offset = 0;
    SerializeBinary(value, buffer, offset, utility::Endianness::BigEndian);
    EXPECT_EQ(std::size_t {4}, offset);
    EXPECT_EQ(offset, buffer.size());
    EXPECT_EQ(std::vector<std::uint8_t>({ 0x3F, 0x00, 0x00, 0x00 }), buffer);
}

TEST(SerializationTest, SerializeBinaryDoubleLittleEndian)
{
    double value = 0.5;
    std::vector<std::uint8_t> buffer;
    std::size_t offset = 0;
    SerializeBinary(value, buffer, offset, utility::Endianness::LittleEndian);
    EXPECT_EQ(std::size_t {8}, offset);
    EXPECT_EQ(offset, buffer.size());
    EXPECT_EQ(std::vector<std::uint8_t>({ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0x3F }), buffer);
}

TEST(SerializationTest, SerializeBinaryDoubleBigEndian)
{
    double value = 0.5;
    std::vector<std::uint8_t> buffer;
    std::size_t offset = 0;
    SerializeBinary(value, buffer, offset, utility::Endianness::BigEndian);
    EXPECT_EQ(std::size_t {8}, offset);
    EXPECT_EQ(offset, buffer.size());
    EXPECT_EQ(std::vector<std::uint8_t>({ 0x3F, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }), buffer);
}

TEST(SerializationTest, SerializeBinaryLongDoubleLittleEndian)
{
    long double value = 0.5L;
    std::vector<std::uint8_t> buffer;
    std::size_t offset = 0;
    SerializeBinary(value, buffer, offset, utility::Endianness::LittleEndian);
    EXPECT_EQ(std::size_t {16}, offset);
    EXPECT_EQ(offset, buffer.size());
    EXPECT_EQ(std::vector<std::uint8_t>({ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xE0, 0x3F, 
                                          0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }), buffer);
}

TEST(SerializationTest, SerializeBinaryLongDoubleBigEndian)
{
    long double value = 0.5L;
    std::vector<std::uint8_t> buffer;
    std::size_t offset = 0;
    SerializeBinary(value, buffer, offset, utility::Endianness::BigEndian);
    EXPECT_EQ(std::size_t {16}, offset);
    EXPECT_EQ(offset, buffer.size());
    EXPECT_EQ(std::vector<std::uint8_t>({ 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                                          0x3F, 0xE0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 }), buffer);
}

TEST(SerializationTest, SerializeBinaryStringLittleEndian)
{
    std::string value = "Hello World!";
    std::vector<std::uint8_t> buffer;
    std::size_t offset = 0;
    SerializeBinary(value, buffer, offset, utility::Endianness::LittleEndian);
    EXPECT_EQ(value.length() + sizeof(size_t), offset);
    EXPECT_EQ(offset, buffer.size());
    std::vector<std::uint8_t> expected;
    if (sizeof(size_t) == 4)
    {
        expected = { 0x0C, 0x00, 0x00, 0x00, 
                     0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x20, 0x57, 0x6F, 
                     0x72, 0x6C, 0x64, 0x21 };
    }
    else
    {
        expected = { 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                     0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x20, 0x57, 0x6F, 
                     0x72, 0x6C, 0x64, 0x21 };
    }
    EXPECT_EQ(expected, buffer);
}

TEST(SerializationTest, SerializeBinaryStringBigEndian)
{
    std::string value = "Hello World!";
    std::vector<std::uint8_t> buffer;
    std::size_t offset = 0;
    SerializeBinary(value, buffer, offset, utility::Endianness::BigEndian);
    EXPECT_EQ(value.length() + sizeof(size_t), offset);
    EXPECT_EQ(offset, buffer.size());
    std::vector<std::uint8_t> expected;
    if (sizeof(size_t) == 4)
    {
        expected = { 0x00, 0x00, 0x00, 0x0C, 
                     0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x20, 0x57, 0x6F, 
                     0x72, 0x6C, 0x64, 0x21 };
    }
    else
    {
        expected = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 
                     0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x20, 0x57, 0x6F, 
                     0x72, 0x6C, 0x64, 0x21 };
    }
    EXPECT_EQ(expected, buffer);
}

TEST(SerializationTest, SerializeBinaryWStringLittleEndian)
{
    std::wstring value = L"Hello World!";
    std::vector<std::uint8_t> buffer;
    std::size_t offset = 0;
    SerializeBinary(value, buffer, offset, utility::Endianness::LittleEndian);
    EXPECT_EQ(value.length() * sizeof(wchar_t) + sizeof(size_t), offset);
    EXPECT_EQ(offset, buffer.size());
    std::vector<std::uint8_t> expected;
    if (sizeof(size_t) == 4)
    {
        expected = { 0x0C, 0x00, 0x00, 0x00 };
    }
    else
    {
        expected = { 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    }

    if (sizeof(wchar_t) == 2)
    {
        expected.insert(expected.end(), { 0x48, 0x00, 0x65, 0x00, 0x6C, 0x00, 0x6C, 0x00, 
                                          0x6F, 0x00, 0x20, 0x00, 0x57, 0x00, 0x6F, 0x00, 
                                          0x72, 0x00, 0x6C, 0x00, 0x64, 0x00, 0x21, 0x00 });
    }
    else
    {
        expected.insert(expected.end(), { 0x48, 0x00, 0x00, 0x00, 0x65, 0x00, 0x00, 0x00, 
                                          0x6C, 0x00, 0x00, 0x00, 0x6C, 0x00, 0x00, 0x00, 
                                          0x6F, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 
                                          0x57, 0x00, 0x00, 0x00, 0x6F, 0x00, 0x00, 0x00, 
                                          0x72, 0x00, 0x00, 0x00, 0x6C, 0x00, 0x00, 0x00, 
                                          0x64, 0x00, 0x00, 0x00, 0x21, 0x00, 0x00, 0x00 });
    }
    EXPECT_EQ(expected, buffer);
}

TEST(SerializationTest, SerializeBinaryWStringBigEndian)
{
    std::wstring value = L"Hello World!";
    std::vector<std::uint8_t> buffer;
    std::size_t offset = 0;
    SerializeBinary(value, buffer, offset, utility::Endianness::BigEndian);
    EXPECT_EQ(value.length() * sizeof(wchar_t) + sizeof(size_t), offset);
    EXPECT_EQ(offset, buffer.size());
    std::vector<std::uint8_t> expected;
    if (sizeof(size_t) == 4)
    {
        expected = { 0x00, 0x00, 0x00, 0x0C };
    }
    else
    {
        expected = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C };
    }
    
    if (sizeof(wchar_t) == 2)
    {
        expected.insert(expected.end(), { 0x00, 0x48, 0x00, 0x65, 0x00, 0x6C, 0x00, 0x6C, 
                                          0x00, 0x6F, 0x00, 0x20, 0x00, 0x57, 0x00, 0x6F, 
                                          0x00, 0x72, 0x00, 0x6C, 0x00, 0x64, 0x00, 0x21 });
    }
    else
    {
        expected.insert(expected.end(), { 0x00, 0x00, 0x00, 0x48, 0x00, 0x00, 0x00, 0x65, 
                                          0x00, 0x00, 0x00, 0x6C, 0x00, 0x00, 0x00, 0x6C, 
                                          0x00, 0x00, 0x00, 0x6F, 0x00, 0x00, 0x00, 0x20, 
                                          0x00, 0x00, 0x00, 0x57, 0x00, 0x00, 0x00, 0x6F, 
                                          0x00, 0x00, 0x00, 0x72, 0x00, 0x00, 0x00, 0x6C, 
                                          0x00, 0x00, 0x00, 0x64, 0x00, 0x00, 0x00, 0x21 });
    }

    EXPECT_EQ(expected, buffer);
}

TEST(SerializationTest, SerializeBinaryCharPtrLittleEndian)
{
    const char * value = "Hello World!";
    std::vector<std::uint8_t> buffer;
    std::size_t offset = 0;
    SerializeBinary(value, buffer, offset, utility::Endianness::LittleEndian);
    EXPECT_EQ(std::strlen(value) + sizeof(size_t), offset);
    EXPECT_EQ(offset, buffer.size());
    std::vector<std::uint8_t> expected;
    if (sizeof(size_t) == 4)
    {
        expected = { 0x0C, 0x00, 0x00, 0x00, 
                     0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x20, 0x57, 0x6F, 
                     0x72, 0x6C, 0x64, 0x21 };
    }
    else
    {
        expected = { 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
                     0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x20, 0x57, 0x6F, 
                     0x72, 0x6C, 0x64, 0x21 };
    }
    EXPECT_EQ(expected, buffer);
}

TEST(SerializationTest, SerializeBinaryCharPtrBigEndian)
{
    const char * value = "Hello World!";
    std::vector<std::uint8_t> buffer;
    std::size_t offset = 0;
    SerializeBinary(value, buffer, offset, utility::Endianness::BigEndian);
    EXPECT_EQ(std::strlen(value) + sizeof(size_t), offset);
    EXPECT_EQ(offset, buffer.size());
    std::vector<std::uint8_t> expected;
    if (sizeof(size_t) == 4)
    {
        expected = { 0x00, 0x00, 0x00, 0x0C, 
                     0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x20, 0x57, 0x6F, 
                     0x72, 0x6C, 0x64, 0x21 };
    }
    else
    {
        expected = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C, 
                     0x48, 0x65, 0x6C, 0x6C, 0x6F, 0x20, 0x57, 0x6F, 
                     0x72, 0x6C, 0x64, 0x21 };
    }
    EXPECT_EQ(expected, buffer);
}

TEST(SerializationTest, SerializeBinaryWCharPtrLittleEndian)
{
    const wchar_t * value = L"Hello World!";
    std::vector<std::uint8_t> buffer;
    std::size_t offset = 0;
    SerializeBinary(value, buffer, offset, utility::Endianness::LittleEndian);
    EXPECT_EQ(std::wcslen(value) * sizeof(wchar_t) + sizeof(size_t), offset);
    EXPECT_EQ(offset, buffer.size());
    std::vector<std::uint8_t> expected;
    if (sizeof(size_t) == 4)
    {
        expected = { 0x0C, 0x00, 0x00, 0x00 };
    }
    else
    {
        expected = { 0x0C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    }

    if (sizeof(wchar_t) == 2)
    {
        expected.insert(expected.end(), { 0x48, 0x00, 0x65, 0x00, 0x6C, 0x00, 0x6C, 0x00, 
                                          0x6F, 0x00, 0x20, 0x00, 0x57, 0x00, 0x6F, 0x00, 
                                          0x72, 0x00, 0x6C, 0x00, 0x64, 0x00, 0x21, 0x00 });
    }
    else
    {
        expected.insert(expected.end(), { 0x48, 0x00, 0x00, 0x00, 0x65, 0x00, 0x00, 0x00, 
                                          0x6C, 0x00, 0x00, 0x00, 0x6C, 0x00, 0x00, 0x00, 
                                          0x6F, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 
                                          0x57, 0x00, 0x00, 0x00, 0x6F, 0x00, 0x00, 0x00, 
                                          0x72, 0x00, 0x00, 0x00, 0x6C, 0x00, 0x00, 0x00, 
                                          0x64, 0x00, 0x00, 0x00, 0x21, 0x00, 0x00, 0x00 });
    }

    EXPECT_EQ(expected, buffer);
}

TEST(SerializationTest, SerializeBinaryWCharPtrBigEndian)
{
    const wchar_t * value = L"Hello World!";
    std::vector<std::uint8_t> buffer;
    std::size_t offset = 0;
    SerializeBinary(value, buffer, offset, utility::Endianness::BigEndian);
    EXPECT_EQ(std::wcslen(value) * sizeof(wchar_t) + sizeof(size_t), offset);
    EXPECT_EQ(offset, buffer.size());
    std::vector<std::uint8_t> expected;
    if (sizeof(size_t) == 4)
    {
        expected = { 0x00, 0x00, 0x00, 0x0C };
    }
    else
    {
        expected = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0C };
    }

    if (sizeof(wchar_t) == 2)
    {
        expected.insert(expected.end(), { 0x00, 0x48, 0x00, 0x65, 0x00, 0x6C, 0x00, 0x6C,
                                          0x00, 0x6F, 0x00, 0x20, 0x00, 0x57, 0x00, 0x6F, 
                                          0x00, 0x72, 0x00, 0x6C, 0x00, 0x64, 0x00, 0x21 });
    }
    else
    {
        expected.insert(expected.end(), { 0x00, 0x00, 0x00, 0x48, 0x00, 0x00, 0x00, 0x65, 
                                          0x00, 0x00, 0x00, 0x6C, 0x00, 0x00, 0x00, 0x6C,
                                          0x00, 0x00, 0x00, 0x6F, 0x00, 0x00, 0x00, 0x20, 
                                          0x00, 0x00, 0x00, 0x57, 0x00, 0x00, 0x00, 0x6F, 
                                          0x00, 0x00, 0x00, 0x72, 0x00, 0x00, 0x00, 0x6C, 
                                          0x00, 0x00, 0x00, 0x64, 0x00, 0x00, 0x00, 0x21 });
    }

    EXPECT_EQ(expected, buffer);
}

namespace {

    enum class TestBool : uint16_t
    {
        True, // = 0
        False // = 1
    };

}

TEST(SerializationTest, SerializeBinaryEnumLittleEndian)
{
    std::vector<std::uint8_t> buffer;
    std::size_t offset = 0;
    SerializeBinary(TestBool::True, buffer, offset, utility::Endianness::LittleEndian);
    SerializeBinary(TestBool::False, buffer, offset, utility::Endianness::LittleEndian);
    EXPECT_EQ(std::size_t {4}, offset);
    EXPECT_EQ(offset, buffer.size());
    EXPECT_EQ(std::vector<std::uint8_t>({ 0x00, 0x00, 0x01, 0x00 }), buffer);
}

TEST(SerializationTest, SerializeBinaryEnumBigEndian)
{
    std::vector<std::uint8_t> buffer;
    std::size_t offset = 0;
    SerializeBinary(TestBool::True, buffer, offset, utility::Endianness::BigEndian);
    SerializeBinary(TestBool::False, buffer, offset, utility::Endianness::BigEndian);
    EXPECT_EQ(std::size_t {4}, offset);
    EXPECT_EQ(offset, buffer.size());
    EXPECT_EQ(std::vector<std::uint8_t>({ 0x00, 0x00, 0x00, 0x01 }), buffer);
}

} // namespace serialization
