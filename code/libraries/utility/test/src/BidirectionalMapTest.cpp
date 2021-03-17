//------------------------------------------------------------------------------
// Copyright   : Copyright(c) 2020, Koninklijke Philips Electronics N.V.
//
// File        : BidirectionalMapTest.cpp
//
// Namespace   : utility
//
// Class       : -
//
// Description : 
//  Test for bi-directional maps
//
//------------------------------------------------------------------------------

#include "GoogleTest.h"

#include "utility/BidirectionalMap.h"

#include <map>
#include <string>

namespace utility {

TEST(BidirectionalMapTest, ForBidirectionalMapOfIntToStringMappingTranslate1EqualsOne)
{
    auto map = BidirectionalMap<int, std::string>
    {
        {1,"one"},
        {2,"two"},
        {3,"three"}
    };
    EXPECT_EQ("one", map.Translate(1));
}

TEST(BidirectionalMapTest, ForBidirectionalMapOfIntToStringMappingTranslateOneEquals1)
{
    auto map = BidirectionalMap<int, std::string>
    {
        { 1,"one" },
        { 2,"two" },
        { 3,"three" }
    };

    EXPECT_EQ(1, map.Translate("one"));
}

TEST(BidirectionalMapTest, ForBidirectionalMapOfIntToStringMappingIndex1EqualsOne)
{
    auto map = BidirectionalMap<int, std::string>
    {
        { 1,"one" },
        { 2,"two" },
        { 3,"three" }
    };

    EXPECT_EQ("one", map[1]);
}

TEST(BidirectionalMapTest, ForBidirectionalMapOfIntToStringMappingIndexOneEquals1)
{
    auto map = BidirectionalMap<int, std::string>
    {
        { 1,"one" },
        { 2,"two" },
        { 3,"three" }
    };

    EXPECT_EQ(1, map["one"]);
}

TEST(BidirectionalMapTest, GetMapReturnsTheForwardTranslationMap)
{
    auto map = BidirectionalMap<int, std::string>
    {
        { 1,"one" },
        { 2,"two" },
        { 3,"three" }
    };

    auto forwardMap = map.GetMap();
    EXPECT_EQ("one", forwardMap[1]);
}

TEST(BidirectionalMapTest, GetKeys)
{
    auto map = BidirectionalMap<int, std::string>
    {
        { 1,"one" },
        { 2,"two" },
        { 3,"three" }
    };

    auto keys = map.GetKeys();
    EXPECT_EQ(keys, std::vector<int>({ 1, 2, 3 }));
}

TEST(BidirectionalMapTest, GetValues)
{
    auto map = BidirectionalMap<int, std::string>
    {
        { 1,"one" },
        { 2,"two" },
        { 3,"three" }
    };

    auto values = map.GetValues();
    EXPECT_EQ(values, std::vector<std::string>({ "one", "two", "three" }));
}

} // namespace utility