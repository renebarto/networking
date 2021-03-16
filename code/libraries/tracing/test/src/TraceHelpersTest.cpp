#include "GoogleTest.h"

#include "tracing/TraceHelpers.h"

namespace tracing {

TEST(TraceHelpersTest, ExtractFileName)
{
    std::string path = "C:\\dir\\file.txt";
    EXPECT_EQ("file.txt", ExtractFileName(path));
}

TEST(TraceHelpersTest, RelativePathSameRoot)
{
    std::string path = __FILE__;
    EXPECT_EQ("code/libraries/tracing/test/src/TraceHelpersTest.cpp", RelativePath(path));
}

TEST(TraceHelpersTest, RelativePathNoSameRoot)
{
    std::string path = "C:\\dir\\file.txt";
    EXPECT_EQ("C:/dir/file.txt", RelativePath(path));
}

} // namespace tracing
