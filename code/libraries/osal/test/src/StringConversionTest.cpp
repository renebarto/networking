#include "GoogleTest.h"

#include "osal/StringConversion.h"

namespace osal
{
    
TEST(StringConversionTest, StringToWString)
{
    EXPECT_EQ(L"ABCDEF", StringToWString("ABCDEF"));
}

TEST(StringConversionTest, WStringToString)
{
    EXPECT_EQ("ABCDEF", WStringToString(L"ABCDEF"));
}

} // namespace osal
