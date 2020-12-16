#include "GoogleTest.h"

#include <string>
#include "json/String.h"

namespace json {
class StringTest: public ::testing::Test
{
public:
    virtual void SetUp();
    virtual void TearDown();
};

void StringTest::SetUp()
{
}

void StringTest::TearDown()
{
}

TEST_F(StringTest, Construct)
{
    String target;

    EXPECT_EQ(ValueType::String, target.Type());
    EXPECT_EQ("", target.GetValue());
}

TEST_F(StringTest, ConstructInitializer)
{
    std::string value("String");
    String target(value);

    EXPECT_EQ(ValueType::String, target.Type());
    EXPECT_EQ(value, target.GetValue());
}

TEST_F(StringTest, Clear)
{
    std::string value("String");
    String target(value);

    EXPECT_EQ(value, target.GetValue());
    target.Clear();
    EXPECT_EQ("", target.GetValue());
}

TEST_F(StringTest, DeserializeEmptyString)
{
    String target;

    EXPECT_EQ(ValueType::String, target.Type());
    EXPECT_EQ("", target.GetValue());
    std::istringstream stream("\"\"");
    EXPECT_TRUE(target.Deserialize(stream));
    EXPECT_EQ("", target.GetValue());
}

TEST_F(StringTest, DeserializeString)
{
    String target;

    std::istringstream stream("\"Some text\"");
    EXPECT_TRUE(target.Deserialize(stream));
    EXPECT_EQ("Some text", target.GetValue());
}

TEST_F(StringTest, DeserializeInvalid)
{
    String target;

    std::istringstream stream("garbage");
    EXPECT_FALSE(target.Deserialize(stream));
    EXPECT_EQ("", target.GetValue());
}

TEST_F(StringTest, DeserializeEmpty)
{
    String target;

    std::istringstream stream("");
    EXPECT_FALSE(target.Deserialize(stream));
    EXPECT_EQ("", target.GetValue());
}

TEST_F(StringTest, DeserializeSpecialChars)
{
    String target;

    std::istringstream stream("\"\\\"\\\\\\/\\b\\f\\n\\r\\t\"");
    EXPECT_TRUE(target.Deserialize(stream));
    EXPECT_EQ("\"\\/\b\f\n\r\t", target.GetValue());
}

TEST_F(StringTest, SerializeEmptyString)
{
    String target;

    std::ostringstream stream;
    target.Serialize(stream);
    EXPECT_EQ("\"\"", stream.str());
}

TEST_F(StringTest, SerializeString)
{
    String target;

    std::ostringstream stream;
    target.SetValue("Some text");
    target.Serialize(stream);
    EXPECT_EQ("\"Some text\"", stream.str());
}

} // namespace json

