//------------------------------------------------------------------------------
// Copyright   : Copyright(c) 2020, Koninklijke Philips Electronics N.V.
//
// File        : MapUtilitiesTest.cpp
//
// Namespace   : utility::maptranslation
//
// Class       : -
//
// Description : 
//  Test for map utilities
//
//------------------------------------------------------------------------------

#include "GoogleTest.h"

#include "utility/MapUtilities.h"

#include <map>
#include <string>

namespace utility {
namespace maptranslation {

TEST(MapUtilityTest, TranslateSimple)
{
    const auto map = std::map<int, std::string>
    {
        { 1, "one" },
        { 2, "two" },
        { 3, "three" }
    };
    const std::string defaultValue("Default");
    EXPECT_EQ(Translate(map, 1), "one");
    EXPECT_EQ(Translate(map, 10, defaultValue), "Default");
}

TEST(MapUtilityTest, TranslateWithDefault)
{
    const auto map = std::map<int, std::string>
    {
        { 1, "one" },
        { 2, "two" },
        { 3, "three" }
    };
    const std::string defaultValue("Default");
    EXPECT_EQ(Translate(map, 10, defaultValue), "Default");
    EXPECT_EQ(Translate(map, 2, defaultValue), "two");
}

} // namespace maptranslation
} // namespace utility
