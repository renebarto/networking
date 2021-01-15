#include "GoogleTest.h"

#include "tracing/TraceHelpers.h"

namespace tracing {

TEST(TraceHelpersTest, ExtractFileName)
{
    std::string path = "C:\\dir\\file.txt";
    EXPECT_EQ("file.txt", ExtractFileName(path));
}

} // namespace tracing
