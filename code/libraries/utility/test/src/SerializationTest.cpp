#include "GoogleTest.h"

#include "utility/Serialization.h"

namespace utility {

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

} // namespace utility
