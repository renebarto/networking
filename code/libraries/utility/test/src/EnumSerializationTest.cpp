#include "utility/EnumSerialization.h"

#include "GoogleTest.h"

#include "tracing/Tracing.h"

namespace core {

TEST(EnumSerializationTest, TraceCategory)
{
    EXPECT_EQ("FuncBeg", utility::Serialize(tracing::TraceCategory::FunctionBegin));
    EXPECT_EQ("FuncEnd", utility::Serialize(tracing::TraceCategory::FunctionEnd));
    EXPECT_EQ("Info", utility::Serialize(tracing::TraceCategory::Information));
    EXPECT_EQ("Log", utility::Serialize(tracing::TraceCategory::Log));
    EXPECT_EQ("Startup", utility::Serialize(tracing::TraceCategory::Startup));
    EXPECT_EQ("Shutdown", utility::Serialize(tracing::TraceCategory::Shutdown));
    EXPECT_EQ("Debug", utility::Serialize(tracing::TraceCategory::Debug));
    EXPECT_EQ("Error", utility::Serialize(tracing::TraceCategory::Error));
}

enum class TestBool
{
    True,
    False
};

} // namespace core

namespace utility {

template<>
const BidirectionalMap<core::TestBool, std::string> EnumSerializationMap<core::TestBool>::ConversionMap = 
{
    { core::TestBool::True, "True"},
    { core::TestBool::False, "False"},
};

} // namespace utility

namespace core {

TEST(EnumSerializationTest, SerializeTestBool)
{
    EXPECT_EQ("True", utility::Serialize(TestBool::True));
    EXPECT_EQ("False", utility::Serialize(TestBool::False));
}

TEST(EnumSerializationTest, DeserializeTestBool)
{
    TestBool value;
    EXPECT_TRUE(utility::Deserialize("True", value));
    EXPECT_EQ(TestBool::True, value);
    EXPECT_TRUE(utility::Deserialize("False", value));
    EXPECT_EQ(TestBool::False, value);
}

} // namespace core
