#include "GoogleTest.h"

#include <string>
//#include "xml/TestData.h"
#include "Parse.h"
#include "json/Boolean.h"
#include "json/String.h"
#include "json/Number.h"
#include "json/Null.h"
#include "json/Object.h"
#include "json/Array.h"

namespace json {
class ParseTest: public ::testing::Test
{
public:
    virtual void SetUp();
    virtual void TearDown();
};

void ParseTest::SetUp()
{
}

void ParseTest::TearDown()
{
}

TEST_F(ParseTest, ParseKeywords)
{
    std::istringstream stream("false true null garbage");
    ValuePtr target = Parse(stream);
    EXPECT_NOT_NULL(target);
    EXPECT_EQ(ValueType::Boolean, target->Type());
    ASSERT_NOT_NULL(std::dynamic_pointer_cast<Boolean>(target));
    EXPECT_FALSE(std::static_pointer_cast<Boolean>(target)->GetValue());

    target = Parse(stream);
    EXPECT_NOT_NULL(target);
    EXPECT_EQ(ValueType::Boolean, target->Type());
    ASSERT_NOT_NULL(std::dynamic_pointer_cast<Boolean>(target));
    EXPECT_TRUE(std::static_pointer_cast<Boolean>(target)->GetValue());

    target = Parse(stream);
    EXPECT_NOT_NULL(target);
    EXPECT_EQ(ValueType::Null, target->Type());
    ASSERT_NOT_NULL(std::dynamic_pointer_cast<Null>(target));

    target = Parse(stream);
    EXPECT_NULL(target);
}

TEST_F(ParseTest, ParseString)
{
    std::istringstream stream("\"false true null garbage\" \"\" ");
    ValuePtr target = Parse(stream);
    EXPECT_NOT_NULL(target);
    EXPECT_EQ(ValueType::String, target->Type());
    ASSERT_NOT_NULL(std::dynamic_pointer_cast<String>(target));
    EXPECT_EQ("false true null garbage", std::static_pointer_cast<String>(target)->GetValue());

    target = Parse(stream);
    EXPECT_NOT_NULL(target);
    EXPECT_EQ(ValueType::String, target->Type());
    ASSERT_NOT_NULL(std::dynamic_pointer_cast<String>(target));
    EXPECT_EQ("", std::static_pointer_cast<String>(target)->GetValue());

    target = Parse(stream);
    EXPECT_NULL(target);
}

TEST_F(ParseTest, ParseNumber)
{
    std::istringstream stream("0123 123.456 1.001e+03 abc");
    ValuePtr target = Parse(stream);
    EXPECT_NOT_NULL(target);
    EXPECT_EQ(ValueType::Number, target->Type());
    EXPECT_EQ("0123", std::static_pointer_cast<Number>(target)->GetValue());

    target = Parse(stream);
    EXPECT_NOT_NULL(target);
    EXPECT_EQ(ValueType::Number, target->Type());
    ASSERT_NOT_NULL(std::dynamic_pointer_cast<Number>(target));
    EXPECT_EQ("123.456", std::static_pointer_cast<Number>(target)->GetValue());

    target = Parse(stream);
    EXPECT_NOT_NULL(target);
    EXPECT_EQ(ValueType::Number, target->Type());
    ASSERT_NOT_NULL(std::dynamic_pointer_cast<Number>(target));
    EXPECT_EQ("1.001e+03", std::static_pointer_cast<Number>(target)->GetValue());

    target = Parse(stream);
    EXPECT_NULL(target);
}

TEST_F(ParseTest, ParseObject)
{
    std::istringstream stream("{ \"x\":true, \"y\":1234 }");
    ValuePtr target = Parse(stream);
    ASSERT_NOT_NULL(target);
    ASSERT_EQ(ValueType::Object, target->Type());
    auto object = std::static_pointer_cast<Object>(target);
    ASSERT_NOT_NULL(object);
    EXPECT_EQ(size_t(2), object->Size());
    auto iterator = object->GetIterator();
    EXPECT_EQ("x", (*iterator).GetKey());
    EXPECT_EQ(ValueType::Boolean, (*iterator).GetValue()->Type());
    ASSERT_NOT_NULL(std::dynamic_pointer_cast<Boolean>((*iterator).GetValue()));
    EXPECT_TRUE(std::dynamic_pointer_cast<Boolean>((*iterator).GetValue())->GetValue());
    iterator++;
    EXPECT_EQ("y", (*iterator).GetKey());
    EXPECT_EQ(ValueType::Number, (*iterator).GetValue()->Type());
    ASSERT_NOT_NULL(std::dynamic_pointer_cast<Number>((*iterator).GetValue()));
    EXPECT_EQ("1234", std::dynamic_pointer_cast<Number>((*iterator).GetValue())->GetValue());
}

TEST_F(ParseTest, ParseArray)
{
    std::istringstream stream("[ true, 1234 ]");
    ValuePtr target = Parse(stream);
    ASSERT_NOT_NULL(target);
    ASSERT_EQ(ValueType::Array, target->Type());
    auto array = std::static_pointer_cast<Array>(target);
    ASSERT_NOT_NULL(array);
    EXPECT_EQ(size_t(2), array->Size());
    auto iterator = array->GetIterator();
    EXPECT_EQ(ValueType::Boolean, (*iterator)->Type());
    ASSERT_NOT_NULL(std::dynamic_pointer_cast<Boolean>(*iterator));
    EXPECT_TRUE(std::dynamic_pointer_cast<Boolean>(*iterator)->GetValue());
    iterator++;
    EXPECT_EQ(ValueType::Number, (*iterator)->Type());
    ASSERT_NOT_NULL(std::dynamic_pointer_cast<Number>(*iterator));
    EXPECT_EQ("1234", std::dynamic_pointer_cast<Number>(*iterator)->GetValue());
}

} // namespace json

