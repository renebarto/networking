#include "GoogleTest.h"

#include <string>
#include "json/Null.h"

namespace json {

class NullTest: public ::testing::Test
{
public:
    virtual void SetUp();
    virtual void TearDown();
};

void NullTest::SetUp()
{
}

void NullTest::TearDown()
{
}

TEST_F(NullTest, Construct)
{
    Null target;

    EXPECT_EQ(ValueType::Null, target.Type());
}

TEST_F(NullTest, Deserialize)
{
    Null target;

    EXPECT_EQ(ValueType::Null, target.Type());
    std::istringstream stream("null");
    EXPECT_TRUE(target.Deserialize(stream));
    stream.str("garbage");
    EXPECT_FALSE(target.Deserialize(stream));
    stream.str("");
    EXPECT_FALSE(target.Deserialize(stream));
}

TEST_F(NullTest, Serialize)
{
    Null target;

    EXPECT_EQ(ValueType::Null, target.Type());
    std::ostringstream stream;
    target.Serialize(stream);
    EXPECT_EQ("null", stream.str());
}

} // namespace json

