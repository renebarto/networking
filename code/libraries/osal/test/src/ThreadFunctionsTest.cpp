//------------------------------------------------------------------------------
// Copyright   : Copyright(c) 2020, Koninklijke Philips Electronics N.V.
//
// File        : ThreadTest.cpp
//
// Namespace   : osal
//
// Class       : -
//
// Description :
//
//------------------------------------------------------------------------------

#include "GoogleTest.h"

#include <sstream>
#include "osal/ThreadFunctions.h"

namespace osal
{
    
TEST(ThreadTest, ByDefaultNoThreadsMapping)
{
    std::ostringstream stream;
    stream << "Thread #" << std::this_thread::get_id();
    std::string text = stream.str();

    EXPECT_EQ(text, GetThreadNameSelf());
}

TEST(ThreadTest, IfTheMainThreadIsNameItIsInTheMap)
{
    SetThreadNameSelf("Main");

    EXPECT_EQ("Main", GetThreadNameSelf());
}

} // namespace osal

