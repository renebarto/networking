//------------------------------------------------------------------------------
// Copyright   : Copyright(c) 2020 Koninklijke Philips Electronics N.V.
//
// File        : EnumSerializationTest.cpp
//
// Namespace   : utility
//
// Class       : -
//
// Description :
//
//------------------------------------------------------------------------------

#include "utility/EnumSerialization.h"

#include "GoogleTest.h"

namespace {

enum class TestBool
{
    True,
    False
};

} // namespace anonymous

namespace serialization {

template<>
const utility::BidirectionalMap<TestBool, std::string> EnumSerializationMap<TestBool>::ConversionMap = 
{
    { TestBool::True, "True"},
    { TestBool::False, "False"},
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
