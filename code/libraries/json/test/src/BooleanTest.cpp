#include "GoogleTest.h"

#include <string>
#include "json/Boolean.h"

namespace json {
class BooleanTest: public ::testing::Test
{
public:
    virtual void SetUp();
    virtual void TearDown();
};

void BooleanTest::SetUp()
{
}

void BooleanTest::TearDown()
{
}

TEST_F(BooleanTest, Construct)
{
    Boolean target;

    EXPECT_EQ(ValueType::Boolean, target.Type());
    EXPECT_FALSE(target.GetValue());
}

TEST_F(BooleanTest, ConstructInitializer)
{
    bool value = true;
    Boolean target(value);

    EXPECT_EQ(ValueType::Boolean, target.Type());
    EXPECT_EQ(value, target.GetValue());
}

TEST_F(BooleanTest, DeserializeFalse)
{
    Boolean target;

    EXPECT_EQ(ValueType::Boolean, target.Type());
    EXPECT_FALSE(target.GetValue());
    std::istringstream stream("false");
    EXPECT_TRUE(target.Deserialize(stream));
    EXPECT_FALSE(target.GetValue());
}

TEST_F(BooleanTest, DeserializeTrue)
{
    Boolean target;

    EXPECT_EQ(ValueType::Boolean, target.Type());
    EXPECT_FALSE(target.GetValue());
    std::istringstream stream("true");
    EXPECT_TRUE(target.Deserialize(stream));
    EXPECT_TRUE(target.GetValue());
}

TEST_F(BooleanTest, DeserializeInvalid)
{
    Boolean target;

    EXPECT_EQ(ValueType::Boolean, target.Type());
    EXPECT_FALSE(target.GetValue());
    std::istringstream stream("garbage");
    EXPECT_FALSE(target.Deserialize(stream));
    EXPECT_FALSE(target.GetValue());
}

TEST_F(BooleanTest, DeserializeEmpty)
{
    Boolean target;

    EXPECT_EQ(ValueType::Boolean, target.Type());
    EXPECT_FALSE(target.GetValue());
    std::istringstream stream("");
    EXPECT_FALSE(target.Deserialize(stream));
    EXPECT_FALSE(target.GetValue());
}

TEST_F(BooleanTest, SerializeFalse)
{
    Boolean target;

    std::ostringstream stream;
    target.SetValue(false);
    target.Serialize(stream);
    EXPECT_EQ("false", stream.str());
}

TEST_F(BooleanTest, SerializeTrue)
{
    Boolean target;

    std::ostringstream stream;
    target.SetValue(true);
    target.Serialize(stream);
    EXPECT_EQ("true", stream.str());
}

} // namespace json

