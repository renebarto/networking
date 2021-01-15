#include "utility/EnumSerialization.h"

#include "GoogleTest.h"

#include "tracing/Tracing.h"

namespace utility {

enum class TestBool
{
    True,
    False
};

} // namespace utility

namespace serialization {

template<>
const utility::BidirectionalMap<utility::TestBool, std::string> EnumSerializationMap<utility::TestBool>::ConversionMap = 
{
    { utility::TestBool::True, "True"},
    { utility::TestBool::False, "False"},
};

} // namespace serialization

namespace utility {

TEST(EnumSerializationTest, SerializeTestBool)
{
    EXPECT_EQ("True", serialization::Serialize(TestBool::True));
    EXPECT_EQ("False", serialization::Serialize(TestBool::False));
}

TEST(EnumSerializationTest, DeserializeTestBool)
{
    TestBool value;
    EXPECT_TRUE(serialization::Deserialize("True", value));
    EXPECT_EQ(TestBool::True, value);
    EXPECT_TRUE(serialization::Deserialize("False", value));
    EXPECT_EQ(TestBool::False, value);
}

} // namespace utility
