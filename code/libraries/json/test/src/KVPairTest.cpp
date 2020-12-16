#include "GoogleTest.h"

#include <string>
#include "json/Boolean.h"
#include "json/Null.h"
#include "json/Number.h"
#include "json/String.h"
#include "json/Object.h"
#include "json/Array.h"

namespace json {
class KVPairTest: public ::testing::Test
{
public:
    virtual void SetUp();
    virtual void TearDown();
};

void KVPairTest::SetUp()
{
}

void KVPairTest::TearDown()
{
}

TEST_F(KVPairTest, Construct)
{
    KVPair target;

    EXPECT_EQ("", target.GetKey());
    EXPECT_NULL(target.GetValue());
}

TEST_F(KVPairTest, ConstructInitializer)
{
    std::string key("key");
    ValuePtr value = std::make_shared<Boolean>(true);
    KVPair target(key, value);

    EXPECT_EQ(key, target.GetKey());
    ASSERT_NOT_NULL(target.GetValue());
    ASSERT_EQ(ValueType::Boolean, target.GetValue()->Type());
    EXPECT_TRUE(std::dynamic_pointer_cast<Boolean>(target.GetValue())->GetValue());
}

TEST_F(KVPairTest, DeserializeKeyBoolean)
{
    KVPair target;

    std::string key("key");
    std::istringstream stream("\"" + key + "\":true");
    EXPECT_TRUE(target.Deserialize(stream));
    EXPECT_EQ(key, target.GetKey());
    ASSERT_NOT_NULL(target.GetValue());
    ASSERT_EQ(ValueType::Boolean, target.GetValue()->Type());
    EXPECT_TRUE(std::dynamic_pointer_cast<Boolean>(target.GetValue())->GetValue());
}

TEST_F(KVPairTest, DeserializeKeyNull)
{
    KVPair target;

    std::string key("key");
    std::istringstream stream("\"" + key + "\":null");
    EXPECT_TRUE(target.Deserialize(stream));
    EXPECT_EQ(key, target.GetKey());
    ASSERT_NOT_NULL(target.GetValue());
    ASSERT_EQ(ValueType::Null, target.GetValue()->Type());
}

TEST_F(KVPairTest, DeserializeKeyNumber)
{
    KVPair target;

    std::string key("key");
    std::istringstream stream("\"" + key + "\":123");
    EXPECT_TRUE(target.Deserialize(stream));
    EXPECT_EQ(key, target.GetKey());
    ASSERT_NOT_NULL(target.GetValue());
    ASSERT_EQ(ValueType::Number, target.GetValue()->Type());
    EXPECT_EQ("123", std::dynamic_pointer_cast<Number>(target.GetValue())->GetValue());
}

TEST_F(KVPairTest, DeserializeKeyString)
{
    KVPair target;

    std::string key("key");
    std::istringstream stream("\"" + key + "\" : \"Some text\"");
    EXPECT_TRUE(target.Deserialize(stream));
    EXPECT_EQ(key, target.GetKey());
    ASSERT_NOT_NULL(target.GetValue());
    ASSERT_EQ(ValueType::String, target.GetValue()->Type());
    EXPECT_EQ("Some text", std::dynamic_pointer_cast<String>(target.GetValue())->GetValue());
}

TEST_F(KVPairTest, DeserializeKeyObject)
{
    KVPair target;

    std::istringstream stream("\"y\" : { \"z\" : 1234 }");
    EXPECT_TRUE(target.Deserialize(stream));

    EXPECT_EQ("y", target.GetKey());
    ASSERT_NOT_NULL(target.GetValue());
    ASSERT_EQ(ValueType::Object, target.GetValue()->Type());

    auto object = std::dynamic_pointer_cast<Object>(target.GetValue());
    ASSERT_NOT_NULL(object);
    ASSERT_EQ(size_t {1}, object->Size());

    auto iterator = object->GetIterator();
    EXPECT_EQ("z", (*iterator).GetKey());
    ASSERT_NOT_NULL((*iterator).GetValue());
    ASSERT_EQ(ValueType::Number, (*iterator).GetValue()->Type());
    EXPECT_EQ("1234", std::dynamic_pointer_cast<Number>((*iterator).GetValue())->GetValue());
}

TEST_F(KVPairTest, DeserializeKeyArray)
{
    KVPair target;

    std::istringstream stream("\"y\" : [ \"z\", 1234 ]");
    EXPECT_TRUE(target.Deserialize(stream));

    EXPECT_EQ("y", target.GetKey());
    ASSERT_NOT_NULL(target.GetValue());
    ASSERT_EQ(ValueType::Array, target.GetValue()->Type());

    auto object = std::dynamic_pointer_cast<Array>(target.GetValue());
    ASSERT_NOT_NULL(object);
    ASSERT_EQ(size_t {2}, object->Size());

    auto iterator = object->GetIterator();
    ASSERT_EQ(ValueType::String, (*iterator)->Type());
    EXPECT_EQ("z", std::dynamic_pointer_cast<String>(*iterator)->GetValue());
    iterator++;
    ASSERT_EQ(ValueType::Number, (*iterator)->Type());
    EXPECT_EQ("1234", std::dynamic_pointer_cast<Number>(*iterator)->GetValue());
}

TEST_F(KVPairTest, DeserializeKeyOnly)
{
    KVPair target;

    std::string key("key");
    std::istringstream stream("\"" + key + "\":");
    EXPECT_FALSE(target.Deserialize(stream));
    EXPECT_EQ("", target.GetKey());
    EXPECT_NULL(target.GetValue());
}

TEST_F(KVPairTest, DeserializeUnquotedKey)
{
    KVPair target;

    std::string key("key");
    std::istringstream stream(key + ":false");
    EXPECT_FALSE(target.Deserialize(stream));
    EXPECT_EQ("", target.GetKey());
    EXPECT_NULL(target.GetValue());
}

TEST_F(KVPairTest, DeserializeEmpty)
{
    KVPair target;

    std::string key("key");
    std::istringstream stream("");
    EXPECT_FALSE(target.Deserialize(stream));
    EXPECT_EQ("", target.GetKey());
    EXPECT_NULL(target.GetValue());
}

TEST_F(KVPairTest, SerializeKeyBoolean)
{
    std::string key("key");
    ValuePtr value = std::make_shared<Boolean>(true);
    KVPair target(key, value);

    std::ostringstream stream;
    target.Serialize(stream);
    EXPECT_EQ("\"" + key + "\" : true", stream.str());
}

TEST_F(KVPairTest, SerializeKeyNumber)
{
    std::string key("key");
    std::shared_ptr<Number> value = std::make_shared<Number>();
    value->SetValue(12345678);

    KVPair target(key, value);

    std::ostringstream stream;
    target.Serialize(stream);
    EXPECT_EQ("\"" + key + "\" : 12345678", stream.str());
}

TEST_F(KVPairTest, SerializeKeyString)
{
    std::string key("key");
    ValuePtr value = std::make_shared<String>("Some text");
    KVPair target(key, value);

    std::ostringstream stream;
    target.Serialize(stream);
    EXPECT_EQ("\"" + key + "\" : \"Some text\"", stream.str());
}

TEST_F(KVPairTest, SerializeKeyNoValue)
{
    std::string key("key");
    ValuePtr value = nullptr;
    KVPair target(key, value);

    std::ostringstream stream;
    target.Serialize(stream);
    EXPECT_EQ("\"" + key + "\" : null", stream.str());
}

TEST_F(KVPairTest, SerializeObject)
{
    std::string key("key");
    std::shared_ptr<Object> value = std::make_shared<Object>();
    std::string keySub("keySub");
    ValuePtr valueSub = std::make_shared<Boolean>(true);
    KVPair targetSub(keySub, valueSub);
    value->AddPair(targetSub);
    KVPair target(key, value);

    std::ostringstream stream;
    target.Serialize(stream);
    EXPECT_EQ("\"" + key + "\" : {\n    \"" + keySub + "\" : true\n" + "}", stream.str());
}

TEST_F(KVPairTest, SerializeArray)
{
    std::string key("key");
    std::shared_ptr<Array> value = std::make_shared<Array>();
    ValuePtr valueSub = std::make_shared<Boolean>(true);
    value->AddValue(valueSub);
    KVPair target(key, value);

    std::ostringstream stream;
    target.Serialize(stream);
    EXPECT_EQ("\"" + key + "\" : [\n    true\n" + "]", stream.str());
}

} // namespace json

