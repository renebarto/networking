#include "GoogleTest.h"

#include <string>
#include "json/Serialization.h"
#include "json/Array.h"
#include "json/Boolean.h"
#include "json/Null.h"
#include "json/Number.h"
#include "json/Object.h"
#include "json/String.h"

namespace json {

class SerializationTest: public ::testing::Test
{
public:
    virtual void SetUp() {}
    virtual void TearDown() {}
};

class MyClass {
public:
    int x;
    int y;

    MyClass()
        : x()
        , y()
    {}
    MyClass(int x, int y)
        : x(x)
        , y(y)
    {}

    bool operator == (const MyClass & other) const
    {
        return (x == other.x) && (y == other.y);
    }
};

template <>
ValuePtr CreateObject(const MyClass & value)
{
    auto object = std::make_shared<Object>();
    object->AddPair("x", CreateValue(value.x));
    object->AddPair("y", CreateValue(value.y));
    return object;
}

std::string Serialize(const MyClass & value)
{
    auto object = CreateObject(value);
    std::ostringstream stream;
    object->Serialize(stream, 0);
    return stream.str();
}

bool Deserialize(const std::string & text, MyClass & value)
{
    Object object;
    bool result = Deserialize(text, object);
    if (result)
    {
        auto xValue = std::dynamic_pointer_cast<Number>(object.Find("x"));
        auto yValue = std::dynamic_pointer_cast<Number>(object.Find("y"));
        if ((xValue != nullptr) && (yValue != nullptr))
        {
            xValue->GetValue(value.x);
            yValue->GetValue(value.y);
        }
        else
            return false;
    }
    return result;
}

class MyComplexClass {
public:
    int x;
    int y;
    std::vector<int> values;
    MyClass sub;

    MyComplexClass()
        : x()
        , y()
        , values()
        , sub()
    {
    }
    MyComplexClass(int x, int y)
        : x(x)
        , y(y)
        , values()
        , sub(x, y)
    {
        values.push_back(x);
        values.push_back(y);
    }
    bool operator == (const MyComplexClass & other) const 
    {
        return 
            (x == other.x) && (y == other.y) &&
            (values == other.values) &&
            (sub == other.sub);
    }
};

template <>
ValuePtr CreateObject(const MyComplexClass & value)
{
    auto object = std::make_shared<Object>();
    object->AddPair("x", CreateValue(value.x));
    object->AddPair("y", CreateValue(value.y));
    object->AddPair("values", CreateArray(value.values));
    object->AddPair("sub", CreateObject(value.sub));
    return object;
}

std::string Serialize(const MyComplexClass & value)
{
    auto object = CreateObject(value);
    std::ostringstream stream;
    object->Serialize(stream, 0);
    return stream.str();
}

bool Deserialize(const std::string & text, MyComplexClass & value)
{
    Object object;
    bool result = Deserialize(text, object);
    if (result)
    {
        auto xValue = std::dynamic_pointer_cast<Number>(object.Find("x"));
        auto yValue = std::dynamic_pointer_cast<Number>(object.Find("y"));
        auto valuesValue = std::dynamic_pointer_cast<Array>(object.Find("values"));
        auto subValue = std::dynamic_pointer_cast<Object>(object.Find("sub"));
        if ((xValue != nullptr) && (yValue != nullptr) && (valuesValue != nullptr) && (subValue != nullptr))
        {
            xValue->GetValue(value.x);
            yValue->GetValue(value.y);
            auto valuesIt = valuesValue->GetIterator();
            while (!valuesIt.AtEnd())
            {
                int v;
                std::dynamic_pointer_cast<Number>(*valuesIt)->GetValue(v);
                value.values.push_back(v);
                valuesIt++;
            }
            auto subXValue = std::dynamic_pointer_cast<Number>(subValue->Find("x"));
            auto subYValue = std::dynamic_pointer_cast<Number>(subValue->Find("y"));
            if ((subXValue != nullptr) && (subYValue != nullptr))
            {
                subXValue->GetValue(value.sub.x);
                subYValue->GetValue(value.sub.y);
            }
            else
                return false;
        }
        else
            return false;
    }
    return result;
}

TEST_F(SerializationTest, SerializeBoolean)
{
    Serializer<Boolean> serializer;

    std::string actual;
    serializer.Serialize(actual, Boolean(true));
    std::string expected = "true";
    EXPECT_EQ(expected, actual);

    serializer.Serialize(actual, Boolean(false));
    expected = "false";
    EXPECT_EQ(expected, actual);
}

TEST_F(SerializationTest, SerializeNull)
{
    Serializer<Null> serializer;

    std::string actual;
    serializer.Serialize(actual, Null());
    std::string expected = "null";
    EXPECT_EQ(expected, actual);
}

TEST_F(SerializationTest, SerializeNumber)
{
    Serializer<Number> serializer;

    std::string actual;
    serializer.Serialize(actual, Number(12345));
    std::string expected = "12345";
    EXPECT_EQ(expected, actual);
}

TEST_F(SerializationTest, SerializeString)
{
    Serializer<String> serializer;

    std::string actual;
    serializer.Serialize(actual, String("Hello World"));
    std::string expected = "\"Hello World\"";
    EXPECT_EQ(expected, actual);
}

TEST_F(SerializationTest, SerializeArray)
{
    Serializer<Array> serializer;

    std::string actual;
    Array array;
    ValuePtr value1 = CreateValue(true);
    ValuePtr value2 = CreateValue("Some text");

    serializer.Serialize(actual, array);
    std::string expected = "[\n]";
    EXPECT_EQ(expected, actual);

    array.AddValue(value1);
    serializer.Serialize(actual, array);
    expected = "[\n    true\n]";
    EXPECT_EQ(expected, actual);

    array.AddValue(value2);
    serializer.Serialize(actual, array);
    expected = "[\n    true,\n    \"Some text\"\n]";
    EXPECT_EQ(expected, actual);
}

TEST_F(SerializationTest, SerializeObject)
{
    Serializer<Object> serializer;

    std::string actual;
    Object object;
    std::string key1("key1");
    std::string key2("key2");
    ValuePtr value1 = CreateValue(true);
    ValuePtr value2 = CreateValue(false);

    serializer.Serialize(actual, object);
    std::string expected = "{\n}";
    EXPECT_EQ(expected, actual);

    object.AddPair(key1, value1);
    serializer.Serialize(actual, object);
    expected = "{\n    \"key1\" : true\n}";
    EXPECT_EQ(expected, actual);

    object.AddPair(key2, value2);
    serializer.Serialize(actual, object);
    expected = "{\n    \"key1\" : true,\n    \"key2\" : false\n}";
    EXPECT_EQ(expected, actual);
}

TEST_F(SerializationTest, SerializeAClass)
{
    Serializer<MyClass> serializer;

    MyClass myClass(1, 2);

    std::string actual;
    serializer.Serialize(actual, myClass);
    std::string expected = "{\n    \"x\" : 1,\n    \"y\" : 2\n}";
    EXPECT_EQ(expected, actual);
}

TEST_F(SerializationTest, SerializeAComplexClass)
{
    Serializer<MyComplexClass> serializer;

    MyComplexClass myClass(1, 2);

    std::string actual;
    serializer.Serialize(actual, myClass);
    std::string expected = "{\n    \"sub\" : {\n        \"x\" : 1,\n        \"y\" : 2\n    },\n    \"values\" : [\n        1,\n        2\n    ],\n    \"x\" : 1,\n    \"y\" : 2\n}";
    EXPECT_EQ(expected, actual);
}

TEST_F(SerializationTest, DeserializeBoolean)
{
    Deserializer<Boolean> deserializer;

    Boolean actual;
    std::string text = "true";
    EXPECT_TRUE(deserializer.Deserialize(text, actual));
    Boolean expected = true;
    EXPECT_EQ(expected.GetValue(), actual.GetValue());

    text = "false";
    EXPECT_TRUE(deserializer.Deserialize(text, actual));
    expected = false;
    EXPECT_EQ(expected.GetValue(), actual.GetValue());
}

TEST_F(SerializationTest, DeserializeNull)
{
    Deserializer<Null> deserializer;

    Null actual;
    std::string text = "null";
    EXPECT_TRUE(deserializer.Deserialize(text, actual));
}

TEST_F(SerializationTest, DeserializeNumber)
{
    Deserializer<Number> deserializer;

    Number actual;
    std::string text = "12345";
    EXPECT_TRUE(deserializer.Deserialize(text, actual));
    Number expected = 12345;
    EXPECT_EQ(expected.GetValue(), actual.GetValue());
}

TEST_F(SerializationTest, DeserializeString)
{
    Deserializer<String> deserializer;

    String actual;
    std::string text = "\"Hello World\"";
    EXPECT_TRUE(deserializer.Deserialize(text, actual));
    String expected = "Hello World";
    EXPECT_EQ(expected.GetValue(), actual.GetValue());
}

TEST_F(SerializationTest, DeserializeArray)
{
    Deserializer<Array> deserializer;

    Array actual;

    ValuePtr value1 = CreateValue(true);
    ValuePtr value2 = CreateValue("Some text");

    std::string text = "[\n]";
    EXPECT_TRUE(deserializer.Deserialize(text, actual));
    Array expected;
    EXPECT_EQ(expected.Size(), actual.Size());

    text = "[\n    true\n]";
    actual.Clear();
    expected.AddValue(value1);
    EXPECT_TRUE(deserializer.Deserialize(text, actual));
    EXPECT_EQ(expected.Size(), actual.Size());
    auto actualIt = actual.GetIterator();
    auto expectedIt = expected.GetIterator();
    EXPECT_EQ((*expectedIt)->Type(), (*actualIt)->Type());
    auto actualValue1 = std::dynamic_pointer_cast<Boolean>(*actualIt);
    auto expectedValue1 = std::dynamic_pointer_cast<Boolean>(value1);
    ASSERT_NOT_NULL(actualValue1);
    ASSERT_NOT_NULL(expectedValue1);
    EXPECT_EQ(expectedValue1->GetValue(), actualValue1->GetValue());

    text = "[\n    true,\n    \"Some text\"\n]";
    actual.Clear();
    expected.AddValue(value2);
    EXPECT_TRUE(deserializer.Deserialize(text, actual));
    EXPECT_EQ(expected.Size(), actual.Size());
    actualIt = actual.GetIterator();
    expectedIt = expected.GetIterator();
    EXPECT_EQ((*expectedIt)->Type(), (*actualIt)->Type());
    actualValue1 = std::dynamic_pointer_cast<Boolean>(*actualIt);
    expectedValue1 = std::dynamic_pointer_cast<Boolean>(value1);
    actualIt++;
    auto actualValue2 = std::dynamic_pointer_cast<String>(*actualIt);
    auto expectedValue2 = std::dynamic_pointer_cast<String>(value2);
    ASSERT_NOT_NULL(actualValue1);
    ASSERT_NOT_NULL(expectedValue1);
    ASSERT_NOT_NULL(actualValue2);
    ASSERT_NOT_NULL(expectedValue2);
    EXPECT_EQ(expectedValue1->GetValue(), actualValue1->GetValue());
    EXPECT_EQ(expectedValue2->GetValue(), actualValue2->GetValue());
}

TEST_F(SerializationTest, DeserializeObject)
{
    Deserializer<Object> deserializer;

    Object actual;
    std::string key1("key1");
    std::string key2("key2");
    ValuePtr value1 = CreateValue(true);
    ValuePtr value2 = CreateValue(false);

    std::string text = "{\n}";
    EXPECT_TRUE(deserializer.Deserialize(text, actual));
    Object expected;
    EXPECT_EQ(expected.Size(), actual.Size());

    text = "{\n    \"key1\" : true\n}";
    actual.Clear();
    expected.AddPair(key1, value1);
    EXPECT_TRUE(deserializer.Deserialize(text, actual));
    EXPECT_EQ(expected.Size(), actual.Size());
    auto actualIt = actual.GetIterator();
    auto expectedIt = expected.GetIterator();
    EXPECT_EQ((*expectedIt).GetValue()->Type(), (*actualIt).GetValue()->Type());
    auto actualValue1 = std::dynamic_pointer_cast<Boolean>((*actualIt).GetValue());
    auto expectedValue1 = std::dynamic_pointer_cast<Boolean>(value1);
    ASSERT_NOT_NULL(actualValue1);
    ASSERT_NOT_NULL(expectedValue1);
    EXPECT_EQ(expectedValue1->GetValue(), actualValue1->GetValue());

    text = "{\n    \"key1\" : true,\n    \"key2\" : false\n}";
    actual.Clear();
    expected.AddPair(key2, value2);
    EXPECT_TRUE(deserializer.Deserialize(text, actual));
    EXPECT_EQ(expected.Size(), actual.Size());
    actualIt = actual.GetIterator();
    expectedIt = expected.GetIterator();
    EXPECT_EQ((*expectedIt).GetValue()->Type(), (*actualIt).GetValue()->Type());
    actualValue1 = std::dynamic_pointer_cast<Boolean>((*actualIt).GetValue());
    expectedValue1 = std::dynamic_pointer_cast<Boolean>(value1);
    actualIt++;
    auto actualValue2 = std::dynamic_pointer_cast<Boolean>((*actualIt).GetValue());
    auto expectedValue2 = std::dynamic_pointer_cast<Boolean>(value2);
    ASSERT_NOT_NULL(actualValue1);
    ASSERT_NOT_NULL(expectedValue1);
    ASSERT_NOT_NULL(actualValue2);
    ASSERT_NOT_NULL(expectedValue2);
    EXPECT_EQ(expectedValue1->GetValue(), actualValue1->GetValue());
    EXPECT_EQ(expectedValue2->GetValue(), actualValue2->GetValue());
}

TEST_F(SerializationTest, DeserializeAClass)
{
    Deserializer<MyClass> deserializer;

    MyClass actual;
    MyClass expected(1, 2);

    std::string text = "{\n    \"x\" : 1,\n    \"y\" : 2\n}";
    EXPECT_TRUE(deserializer.Deserialize(text, actual));
    EXPECT_EQ(expected, actual);
}

TEST_F(SerializationTest, DeserializeAComplexClass)
{
    Deserializer<MyComplexClass> deserializer;

    MyComplexClass actual;
    MyComplexClass expected(1, 2);

    std::string text = "{\n    \"sub\" : {\n        \"x\" : 1,\n        \"y\" : 2\n    },\n    \"values\" : [\n        1,\n        2\n    ],\n    \"x\" : 1,\n    \"y\" : 2\n}";
    EXPECT_TRUE(deserializer.Deserialize(text, actual));
    EXPECT_EQ(expected, actual);
}

} // namespace json

