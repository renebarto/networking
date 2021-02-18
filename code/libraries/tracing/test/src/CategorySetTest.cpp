//------------------------------------------------------------------------
// Copyright   : Copyright(c) 2020, Koninklijke Philips Electronics N.V.
//
// File        : CategorySetTest.cpp
//
// Namespace   : tracing
//
// Class       : -
//
// Description :
//  CategorySet tests
//
//---------------------------------------------------------------

#include "GoogleTest.h"

#include <string>
#include "tracing/TraceCategory.h"
#include "tracing/CategorySet.h"

namespace tracing {

TEST(CategorySetTest, ConstructDefault)
{
    CategorySet<TraceCategory> set;
    for (auto const & category : EnumTraceCategories())
    {
        EXPECT_FALSE(set.is_set(category));
    }
}

TEST(CategorySetTest, Construct)
{
    CategorySet<TraceCategory> set(TraceCategory::Message);
    for (auto const & category : EnumTraceCategories())
    {
        if (category != TraceCategory::Message)
            EXPECT_FALSE(set.is_set(category));
        else
            EXPECT_TRUE(set.is_set(category));
    }
}

TEST(CategorySetTest, ConstructMaskNone)
{
    CategorySet<TraceCategory> set(0x0000);
    for (auto const & category : EnumTraceCategories())
    {
        EXPECT_FALSE(set.is_set(category));
    }
}

TEST(CategorySetTest, ConstructMaskAll)
{
    CategorySet<TraceCategory> set(0xFFFF);
    for (auto const & category : EnumTraceCategories())
    {
        EXPECT_TRUE(set.is_set(category));
    }
}

TEST(CategorySetTest, ConstructMaskOnlyDebug)
{
    CategorySet<TraceCategory> set(0x0001);
    for (auto const & category : EnumTraceCategories())
    {
        if (category != TraceCategory::FunctionEnter)
            EXPECT_FALSE(set.is_set(category));
        else
            EXPECT_TRUE(set.is_set(category));
    }
}

TEST(CategorySetTest, Set)
{
    CategorySet<TraceCategory> set;
    set.set(TraceCategory::FunctionEnter);
    set.set(TraceCategory::FunctionLeave);
    for (auto const & category : EnumTraceCategories())
    {
        if ((category != TraceCategory::FunctionEnter) && (category != TraceCategory::FunctionLeave))
            EXPECT_FALSE(set.is_set(category));
        else
            EXPECT_TRUE(set.is_set(category));
    }
}

TEST(CategorySetTest, Reset)
{
    CategorySet<TraceCategory> set(0xFFFF);
    set.reset(TraceCategory::FunctionEnter);
    set.reset(TraceCategory::FunctionLeave);
    for (auto const & category : EnumTraceCategories())
    {
        if ((category != TraceCategory::FunctionEnter) && (category != TraceCategory::FunctionLeave))
            EXPECT_TRUE(set.is_set(category));
        else
            EXPECT_FALSE(set.is_set(category));
    }
}

TEST(CategorySetTest, OperatorOr)
{
    CategorySet<TraceCategory> set;
    set |= TraceCategory::FunctionEnter;
    set |= TraceCategory::FunctionLeave;
    for (auto const & category : EnumTraceCategories())
    {
        if ((category != TraceCategory::FunctionEnter) && (category != TraceCategory::FunctionLeave))
            EXPECT_FALSE(set.is_set(category));
        else
            EXPECT_TRUE(set.is_set(category));
    }
}

} // namespace tracing
