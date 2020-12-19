#include "GoogleTest.h"

#include "utility/Format.h"
#include "utility/BidirectionalMap.h"

namespace utility {

template <class T>
std::string FormatStandard(std::streamsize width, std::streamsize /*integralDigits*/, std::string::size_type /*fractionalDigits*/, T value)
{
    std::ostringstream valuePrint;
    std::ostringstream stream;
    valuePrint << std::setfill('0') << std::setprecision(width) << value;
    if (static_cast<size_t>(width) > valuePrint.str().length())
        stream << std::string(width - valuePrint.str().length(), ' ');
    stream << valuePrint.str();
    return stream.str();
}
template <class T>
std::string FormatFixed(std::streamsize width, std::streamsize integralDigits, std::streamsize fractionalDigits, T value)
{
    std::ostringstream valuePrint;
    std::ostringstream stream;
    valuePrint << std::fixed << std::setfill('0') << std::setw(integralDigits + fractionalDigits + 1) << std::setprecision(fractionalDigits) << value;
    if (static_cast<size_t>(width) > valuePrint.str().length())
        stream << std::string(width - valuePrint.str().length(), ' ');
    stream << valuePrint.str();
    return stream.str();
}
template <class T>
std::string FormatScientific(std::streamsize width, std::streamsize integralDigits, std::streamsize fractionalDigits, T value)
{
    std::ostringstream valuePrint;
    std::ostringstream stream;
    valuePrint << std::scientific << std::setfill('0') << std::setw(integralDigits + fractionalDigits + 1) << std::setprecision(fractionalDigits) << value;
    if (static_cast<size_t>(width) > valuePrint.str().length())
        stream << std::string(width - valuePrint.str().length(), ' ');
    stream << valuePrint.str();
    return stream.str();
}

TEST(FormatTest, FormatCharPtrNarrow)
{
    std::string text = "Hello";
    std::string str;
    Format(str, text);

    EXPECT_EQ(text, str);
}

TEST(FormatTest, FormatCharPtrWide)
{
    std::string text = "Hello";
    const wchar_t * wtext = L"Hello";
    std::string str;
    Format(str, wtext);

    EXPECT_EQ(text, str);
}

TEST(FormatTest, FormatStringNarrow)
{
    std::string text = "Hello";
    std::string str;
    Format(str, text);

    EXPECT_EQ(text, str);
}

TEST(FormatTest, FormatStringWide)
{
    const char * text = "Hello";
    std::wstring wtext = L"Hello";
    std::string str;
    Format(str, wtext);

    EXPECT_EQ(std::string(text), str);
}

TEST(FormatTest, FormatNoFormat)
{
    const std::string text = "";
    std::string str;
    Format(str, "", 123);

    EXPECT_EQ(text, str);
    ASSERT_TRUE(HaveFormatError());
    EXPECT_EQ("Not all parameters have been used. Parameters not used: 0", GetFormatError());
}

TEST(FormatTest, FormatSimple)
{
    const char * text = "123 456";
    std::string str;
    Format(str, "{} {}", 123, 456);

    EXPECT_EQ(std::string(text), str);
}

TEST(FormatTest, FormatIndexes)
{
    const char * text = "123 456 World Hello";
    std::string str;
    Format(str, "{} {} {3} {2}", 123, 456, "Hello", L"World");

    EXPECT_EQ(std::string(text), str);
}

TEST(FormatTest, FormatWithWidth)
{
    const char * text = "       123 456           World Hello   ";
    std::string str;
    Format(str, "{,10} {,-10} {3,8} {2,-8}", 123, 456, "Hello", L"World");

    EXPECT_EQ(std::string(text), str);
}

TEST(FormatTest, FormatIntegralWithFormat)
{
    const char * text = " 305419896    305419896 00010010001101000101011001111000  02215053170   12345678";
    std::string str;
    int value = 0x12345678;
    Format(str, "{0,10:D} {0,12:D10} {0,32:B32} {0,12:O11} {0,10:X8}", value);

    EXPECT_EQ(std::string(text), str);
}

TEST(FormatTest, FormatFloatingPointWithFormat)
{
    // Apparently there is a difference between MinGW and Windows in how they print floating point numbers
    std::ostringstream refStream;
    std::string str;
    double value = 1234.5678;
    Format(str, "{0,10} {0,10:F4.4} {0,10:F3.4} {0,10:F3.3} {0,12:F5.5} {0,10:E4.4} {0,10:G5.4}", value);

    refStream << FormatStandard<double>(10, 0, 0, value);
    refStream << ' ';
    refStream << FormatFixed<double>(10, 4, 4, value);
    refStream << ' ';
    refStream << FormatFixed<double>(10, 3, 4, value);
    refStream << ' ';
    refStream << FormatFixed<double>(10, 3, 3, value);
    refStream << ' ';
    refStream << FormatFixed<double>(12, 5, 5, value);
    refStream << ' ';
    refStream << FormatScientific<double>(10, 4, 4, value);
    refStream << ' ';
    refStream << FormatStandard<double>(10, 5, 4, value);

    EXPECT_EQ(refStream.str(), str);
}

TEST(FormatTest, FormatInvalidIndex)
{
    const std::string text = "";
    std::string str;
    Format(str, "{a}", 123);

    EXPECT_EQ(text, str);
    ASSERT_TRUE(HaveFormatError());
    EXPECT_EQ("Invalid index specified: a", GetFormatError());
}

TEST(FormatTest, FormatIndexOutOfRangeLow)
{
    const std::string text = "";
    std::string str;
    Format(str, "{-1}", 123);

    EXPECT_EQ(text, str);
    ASSERT_TRUE(HaveFormatError());
    EXPECT_EQ("Invalid index specified: -1", GetFormatError());
}

TEST(FormatTest, FormatIndexOutOfRangeHigh)
{
    const std::string text = "";
    std::string str;
    Format(str, "{1}", 123);

    EXPECT_EQ(text, str);
    ASSERT_TRUE(HaveFormatError());
    EXPECT_EQ("Invalid index specified: 1 should be between 0 and 0", GetFormatError());
}

TEST(FormatTest, FormatInvalidWidth)
{
    const std::string text = "";
    std::string str;
    Format(str, "{0,a}", 123);

    EXPECT_EQ(text, str);
    ASSERT_TRUE(HaveFormatError());
    EXPECT_EQ("Invalid width specified: a", GetFormatError());
}

namespace {

enum class MyType {
    Yes,
    No
};

static BidirectionalMap<MyType, std::string> conversionMap { {MyType::Yes, "Yes" }, { MyType::No, "No" } };

std::ostream & operator << (std::ostream & stream, const MyType & value)
{
    return stream << conversionMap.Translate(value);
}

} // anonymous namespace

TEST(FormatTest, FormatEnumClass)
{
    const std::string text = "Yes";
    std::string str;
    Format(str, "{0}", MyType::Yes);

    EXPECT_EQ(text, str);
    ASSERT_FALSE(HaveFormatError());
}

} // namespace utility
