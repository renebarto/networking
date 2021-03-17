//------------------------------------------------------------------------------
// Copyright   : Copyright(c) 2020, Koninklijke Philips Electronics N.V.
//
// File        : ErrorCodeTest.cpp
//
// Namespace   : osal
//
// Class       : -
//
// Description :
//
//------------------------------------------------------------------------------

#include "osal/ErrorCode.h"

#include "GoogleTest.h"

namespace osal {

TEST(ErrorCodeTest, ConstructDefault)
{
    ErrorCode errorCode;
    EXPECT_TRUE(static_cast<bool>(errorCode));
    EXPECT_EQ(0, errorCode.Code());
//TICS -POR#021 Platform specific
#if defined(PLATFORM_WINDOWS)
    std::string errorString = "No error";
#else
    std::string errorString = "Success";
#endif
//TICS +POR#021

    EXPECT_EQ(errorString, errorCode.Description());
}

TEST(ErrorCodeTest, ConstructNoError)
{
    ErrorCode errorCode(0);
    EXPECT_TRUE(static_cast<bool>(errorCode));
    EXPECT_EQ(0, errorCode.Code());
}

TEST(ErrorCodeTest, ConstructError)
{
    ErrorCode errorCode(2); // File not found
    EXPECT_FALSE(static_cast<bool>(errorCode));
    EXPECT_EQ(2, errorCode.Code());
}

} // namespace osal
