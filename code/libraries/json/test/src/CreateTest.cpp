#include "GoogleTest.h"

#include <string>
#include "json/Create.h"

namespace json {

class ValueTest: public ::testing::Test
{
public:
    virtual void SetUp() {}
    virtual void TearDown() {}
};

TEST_F(ValueTest, CreateValueNull)
{
    auto value = CreateValue(nullptr);

    EXPECT_EQ(ValueType::Null, value->Type());
}

TEST_F(ValueTest, CreateValueBoolean)
{
    auto value = CreateValue(true);

    EXPECT_EQ(ValueType::Boolean, value->Type());
    EXPECT_EQ(true, std::dynamic_pointer_cast<Boolean>(value)->GetValue());

    value = CreateValue(false);

    EXPECT_EQ(ValueType::Boolean, value->Type());
    EXPECT_EQ(false, std::dynamic_pointer_cast<Boolean>(value)->GetValue());
}

TEST_F(ValueTest, CreateValueString)
{
    auto value = CreateValue("Hello World");

    EXPECT_EQ(ValueType::String, value->Type());
    EXPECT_EQ("Hello World", std::dynamic_pointer_cast<String>(value)->GetValue());

    value = CreateValue("");

    EXPECT_EQ(ValueType::String, value->Type());
    EXPECT_EQ("", std::dynamic_pointer_cast<String>(value)->GetValue());
}

TEST_F(ValueTest, CreateValueNumberInt8)
{
    auto value = CreateValue(int8_t {127});

    EXPECT_EQ(ValueType::Number, value->Type());
    int8_t expected = 127;
    int8_t actual;
    EXPECT_TRUE(std::dynamic_pointer_cast<Number>(value)->GetValue(actual));
    EXPECT_EQ(expected, actual);
}

TEST_F(ValueTest, CreateValueNumberUInt8)
{
    auto value = CreateValue(uint8_t {255});

    EXPECT_EQ(ValueType::Number, value->Type());
    uint8_t expected = 255;
    uint8_t actual;
    EXPECT_TRUE(std::dynamic_pointer_cast<Number>(value)->GetValue(actual));
    EXPECT_EQ(expected, actual);
}

TEST_F(ValueTest, CreateValueNumberInt16)
{
    auto value = CreateValue(int16_t {32767});

    EXPECT_EQ(ValueType::Number, value->Type());
    int16_t expected = 32767;
    int16_t actual;
    EXPECT_TRUE(std::dynamic_pointer_cast<Number>(value)->GetValue(actual));
    EXPECT_EQ(expected, actual);
}

TEST_F(ValueTest, CreateValueNumberUInt16)
{
    auto value = CreateValue(uint16_t {65535});

    EXPECT_EQ(ValueType::Number, value->Type());
    uint16_t expected = 65535;
    uint16_t actual;
    EXPECT_TRUE(std::dynamic_pointer_cast<Number>(value)->GetValue(actual));
    EXPECT_EQ(expected, actual);
}

TEST_F(ValueTest, CreateValueNumberInt32)
{
    auto value = CreateValue(int32_t {0x7FFFFFFF});

    EXPECT_EQ(ValueType::Number, value->Type());
    int32_t expected = 0x7FFFFFFF;
    int32_t actual;
    EXPECT_TRUE(std::dynamic_pointer_cast<Number>(value)->GetValue(actual));
    EXPECT_EQ(expected, actual);
}

TEST_F(ValueTest, CreateValueNumberUInt32)
{
    auto value = CreateValue(uint32_t {0xFFFFFFFF});

    EXPECT_EQ(ValueType::Number, value->Type());
    uint32_t expected = 0xFFFFFFFF;
    uint32_t actual;
    EXPECT_TRUE(std::dynamic_pointer_cast<Number>(value)->GetValue(actual));
    EXPECT_EQ(expected, actual);
}

TEST_F(ValueTest, CreateValueNumberInt64)
{
    auto value = CreateValue(int64_t {0x7FFFFFFFFFFFFFFF});

    EXPECT_EQ(ValueType::Number, value->Type());
    int64_t expected = 0x7FFFFFFFFFFFFFFF;
    int64_t actual;
    EXPECT_TRUE(std::dynamic_pointer_cast<Number>(value)->GetValue(actual));
    EXPECT_EQ(expected, actual);
}

TEST_F(ValueTest, CreateValueNumberUInt64)
{
    auto value = CreateValue(uint64_t {0xFFFFFFFFFFFFFFFF});

    EXPECT_EQ(ValueType::Number, value->Type());
    uint64_t expected = 0xFFFFFFFFFFFFFFFF;
    uint64_t actual;
    EXPECT_TRUE(std::dynamic_pointer_cast<Number>(value)->GetValue(actual));
    EXPECT_EQ(expected, actual);
}

TEST_F(ValueTest, CreateValueNumberFloat)
{
    auto value = CreateValue(float {0.1234F});

    EXPECT_EQ(ValueType::Number, value->Type());
    float expected = 0.1234F;
    float actual;
    EXPECT_TRUE(std::dynamic_pointer_cast<Number>(value)->GetValue(actual));
    EXPECT_EQ(expected, actual);
}

TEST_F(ValueTest, CreateValueNumberDouble)
{
    auto value = CreateValue(double {0.12345});

    EXPECT_EQ(ValueType::Number, value->Type());
    double expected = 0.12345;
    double actual;
    EXPECT_TRUE(std::dynamic_pointer_cast<Number>(value)->GetValue(actual));
    EXPECT_EQ(expected, actual);
}

} // namespace json

