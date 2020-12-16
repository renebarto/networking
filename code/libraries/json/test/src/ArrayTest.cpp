#include "GoogleTest.h"

#include <string>
#include "json/Boolean.h"
#include "json/Create.h"
#include "json/Null.h"
#include "json/Number.h"
#include "json/String.h"
#include "json/Object.h"
#include "json/Array.h"

namespace json {
class ArrayTest: public ::testing::Test
{
public:
    virtual void SetUp();
    virtual void TearDown();
};

void ArrayTest::SetUp()
{
}

void ArrayTest::TearDown()
{
}

TEST_F(ArrayTest, Construct)
{
    Array target;

    EXPECT_EQ(ValueType::Array, target.Type());
    EXPECT_EQ(size_t {0}, target.Size());
}

TEST_F(ArrayTest, ConstructInitializerSingleValue)
{
    ValuePtr value = CreateValue(true);

    Array target({value});

    EXPECT_EQ(ValueType::Array, target.Type());
    ASSERT_EQ(size_t {1}, target.Size());
    auto iterator = target.GetIterator();
    EXPECT_EQ(value, (*iterator));
}

TEST_F(ArrayTest, ConstructInitializerMultipleValues)
{
    ValuePtr value1 = CreateValue(true);
    ValuePtr value2 = CreateValue("Some text");

    Array target({value1, value2});

    EXPECT_EQ(ValueType::Array, target.Type());
    ASSERT_EQ(size_t {2}, target.Size());
    auto iterator = target.GetIterator();
    EXPECT_EQ(value1, (*iterator));
    ++iterator;
    EXPECT_EQ(value2, (*iterator));
}

TEST_F(ArrayTest, Clear)
{
    ValuePtr value = CreateValue(true);

    Array target({value});

    ASSERT_EQ(size_t {1}, target.Size());
    target.Clear();
    ASSERT_EQ(size_t {0}, target.Size());
}

TEST_F(ArrayTest, AddValue)
{
    Array target;
    ValuePtr value1 = CreateValue(true);
    ValuePtr value2 = CreateValue("Some text");
    ASSERT_EQ(size_t {0}, target.Size());
    target.AddValue(value1);
    ASSERT_EQ(size_t {1}, target.Size());
    target.AddValue(value2);
    ASSERT_EQ(size_t {2}, target.Size());
    auto iterator = target.GetIterator();
    EXPECT_EQ(value1, (*iterator));
    ++iterator;
    EXPECT_EQ(value2, (*iterator));
}

TEST_F(ArrayTest, DeserializeSingleValue)
{
    Array target;

    std::istringstream stream("[true]");
    EXPECT_TRUE(target.Deserialize(stream));
    EXPECT_EQ(ValueType::Array, target.Type());
    ASSERT_EQ(size_t {1}, target.Size());
    auto iterator = target.GetIterator();
    ASSERT_NOT_NULL(*iterator);
    ASSERT_EQ(ValueType::Boolean, (*iterator)->Type());
    EXPECT_TRUE(std::dynamic_pointer_cast<Boolean>(*iterator)->GetValue());
}

TEST_F(ArrayTest, DeserializeMultipleValues)
{
    Array target;

    std::istringstream stream("[\n"
                                                "    true,\n"
                                                "    false,\n"
                                                "    null,\n"
                                                "    12345678\n"
                                                "]");
    EXPECT_TRUE(target.Deserialize(stream));
    EXPECT_EQ(ValueType::Array, target.Type());
    ASSERT_EQ(size_t {4}, target.Size());

    auto iterator = target.GetIterator();
    ASSERT_NOT_NULL(*iterator);
    ASSERT_EQ(ValueType::Boolean, (*iterator)->Type());
    EXPECT_TRUE(std::dynamic_pointer_cast<Boolean>(*iterator)->GetValue());

    ++iterator;
    ASSERT_NOT_NULL(*iterator);
    ASSERT_EQ(ValueType::Boolean, (*iterator)->Type());
    EXPECT_FALSE(std::dynamic_pointer_cast<Boolean>(*iterator)->GetValue());

    ++iterator;
    ASSERT_NOT_NULL(*iterator);
    ASSERT_EQ(ValueType::Null, (*iterator)->Type());

    ++iterator;
    ASSERT_NOT_NULL(*iterator);
    ASSERT_EQ(ValueType::Number, (*iterator)->Type());
    EXPECT_EQ("12345678", std::dynamic_pointer_cast<Number>(*iterator)->GetValue());
}

TEST_F(ArrayTest, DeserializeMultipleValuesForgottenComma)
{
    Array target;

    std::istringstream stream("[\n"
                                                "    true,\n"
                                                "    false,\n"
                                                "    null\n"
                                                "    12345678\n"
                                                "]");
    EXPECT_FALSE(target.Deserialize(stream));
    EXPECT_EQ(ValueType::Array, target.Type());
    ASSERT_EQ(size_t {3}, target.Size());

    auto iterator = target.GetIterator();
    ASSERT_NOT_NULL(*iterator);
    ASSERT_EQ(ValueType::Boolean, (*iterator)->Type());
    EXPECT_TRUE(std::dynamic_pointer_cast<Boolean>(*iterator)->GetValue());

    ++iterator;
    ASSERT_NOT_NULL(*iterator);
    ASSERT_EQ(ValueType::Boolean, (*iterator)->Type());
    EXPECT_FALSE(std::dynamic_pointer_cast<Boolean>(*iterator)->GetValue());

    ++iterator;
    ASSERT_NOT_NULL(*iterator);
    ASSERT_EQ(ValueType::Null, (*iterator)->Type());
}

TEST_F(ArrayTest, DeserializeMultipleValuesExtraComma)
{
    Array target;

    std::istringstream stream("[\n"
                                                "    true,\n"
                                                "    false,\n"
                                                "    null,\n"
                                                "    12345678,\n"
                                                "]");
    EXPECT_FALSE(target.Deserialize(stream));
    EXPECT_EQ(ValueType::Array, target.Type());
    ASSERT_EQ(size_t {4}, target.Size());

    auto iterator = target.GetIterator();
    ASSERT_NOT_NULL(*iterator);
    ASSERT_EQ(ValueType::Boolean, (*iterator)->Type());
    EXPECT_TRUE(std::dynamic_pointer_cast<Boolean>(*iterator)->GetValue());

    ++iterator;
    ASSERT_NOT_NULL(*iterator);
    ASSERT_EQ(ValueType::Boolean, (*iterator)->Type());
    EXPECT_FALSE(std::dynamic_pointer_cast<Boolean>(*iterator)->GetValue());

    ++iterator;
    ASSERT_NOT_NULL(*iterator);
    ASSERT_EQ(ValueType::Null, (*iterator)->Type());

    ++iterator;
    ASSERT_NOT_NULL(*iterator);
    ASSERT_EQ(ValueType::Number, (*iterator)->Type());
    EXPECT_EQ("12345678", std::dynamic_pointer_cast<Number>(*iterator)->GetValue());
}

TEST_F(ArrayTest, DeserializeEmptyArray)
{
    Array target;

    std::istringstream stream("[]");
    EXPECT_TRUE(target.Deserialize(stream));
    EXPECT_EQ(ValueType::Array, target.Type());
    ASSERT_EQ(size_t {0}, target.Size());
}

TEST_F(ArrayTest, DeserializeNestedObject)
{
    Array target;

    std::istringstream stream("[ true, { \"z\" : 1234 }]");
    EXPECT_TRUE(target.Deserialize(stream));
    EXPECT_EQ(ValueType::Array, target.Type());
    ASSERT_EQ(size_t {2}, target.Size());

    auto iterator = target.GetIterator();
    ASSERT_EQ(ValueType::Boolean, (*iterator)->Type());
    EXPECT_TRUE(std::dynamic_pointer_cast<Boolean>(*iterator)->GetValue());

    ++iterator;
    ASSERT_EQ(ValueType::Object, (*iterator)->Type());

    auto object = std::dynamic_pointer_cast<Object>(*iterator);
    ASSERT_NOT_NULL(object);
    ASSERT_EQ(size_t {1}, object->Size());

    auto iteratorNested = object->GetIterator();
    EXPECT_EQ("z", (*iteratorNested).GetKey());
    ASSERT_NOT_NULL((*iteratorNested).GetValue());
    ASSERT_EQ(ValueType::Number, (*iteratorNested).GetValue()->Type());
    EXPECT_EQ("1234", std::dynamic_pointer_cast<Number>((*iteratorNested).GetValue())->GetValue());
}

TEST_F(ArrayTest, DeserializeNestedArray)
{
    Array target;

    std::istringstream stream("[ true, [ \"z\", 1234 ]]");
    EXPECT_TRUE(target.Deserialize(stream));
    EXPECT_EQ(ValueType::Array, target.Type());
    ASSERT_EQ(size_t {2}, target.Size());

    auto iterator = target.GetIterator();
    ASSERT_EQ(ValueType::Boolean, (*iterator)->Type());
    EXPECT_TRUE(std::dynamic_pointer_cast<Boolean>(*iterator)->GetValue());

    ++iterator;
    ASSERT_EQ(ValueType::Array, (*iterator)->Type());

    auto object = std::dynamic_pointer_cast<Array>(*iterator);
    ASSERT_NOT_NULL(object);
    ASSERT_EQ(size_t {2}, object->Size());

    auto iteratorNested = object->GetIterator();
    ASSERT_EQ(ValueType::String, (*iteratorNested)->Type());
    EXPECT_EQ("z", std::dynamic_pointer_cast<String>(*iteratorNested)->GetValue());
    iteratorNested++;
    ASSERT_EQ(ValueType::Number, (*iteratorNested)->Type());
    EXPECT_EQ("1234", std::dynamic_pointer_cast<Number>(*iteratorNested)->GetValue());
}

TEST_F(ArrayTest, Serialize)
{
    ValuePtr value1 = CreateValue(true);
    ValuePtr value2 = CreateValue(false);
    ValuePtr value3 = std::make_shared<Null>();
    ValuePtr value4 = std::make_shared<Number>("12345678");
    auto value5 = std::make_shared<Object>();
    value5->AddPair(KVPair("x", CreateValue(true)));
    auto value6 = std::make_shared<Array>();
    value6->AddValue(CreateValue(true));
    value6->AddValue(CreateValue(false));

    Array target(
    {
        value1,
        value2,
        value3,
        value4,
        value5,
        value6
    });

    std::ostringstream stream;
    target.Serialize(stream);
    EXPECT_EQ("[\n"
              "    true,\n"
              "    false,\n"
              "    null,\n"
              "    12345678,\n"
              "    {\n"
              "        \"x\" : true\n"
              "    },\n"
              "    [\n"
              "        true,\n"
              "        false\n"
              "    ]\n"
              "]", stream.str());
}

TEST_F(ArrayTest, Vector)
{
    std::vector<bool> data { true, false, true };

    auto target = CreateArray(data);

    std::ostringstream stream;
    target->Serialize(stream);
    EXPECT_EQ("[\n"
              "    true,\n"
              "    false,\n"
              "    true\n"
              "]", stream.str());
}

} // namespace json

