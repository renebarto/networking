#pragma once

#include <iostream>

//TICS -POR#021 We suppress warnings for Windows only
#if defined (_MSC_VER)
#pragma warning(disable: 4826) //TICS !POR#018 !POR#037 Suppress compilation warnings on GoogleTest for Windows only
#endif
#include "gtest/gtest.h"
#include "gmock/gmock.h"
#if defined (_MSC_VER)
#pragma warning(default: 4826) //TICS !POR#018 !POR#037 Suppress compilation warnings on GoogleTest for Windows only
#endif
//TICS +POR#021

#define EXPECT_NULL(a) EXPECT_TRUE(nullptr == (a))
#define EXPECT_NOT_NULL(a) EXPECT_FALSE(nullptr == (a))
#define ASSERT_NULL(a) ASSERT_TRUE(nullptr == (a))
#define ASSERT_NOT_NULL(a) ASSERT_FALSE(nullptr == (a))
