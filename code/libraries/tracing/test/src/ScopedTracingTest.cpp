#include "GoogleTest.h"

#include "tracing/ScopedTracing.h"

namespace tracing {

static std::string traceOutput;

static void TraceFunc(
    TraceCategory category,
    const std::string & fileName, 
    int line, 
    const std::string & functionName, 
    const std::string & msg)
{
    std::ostringstream stream;
    stream << category << "|" << fileName << ":" << line << "|" << functionName << "|" << msg << std::endl;
    traceOutput += stream.str();
}

TEST(ScopedTracingTest, IfNothingEnabledNothingHappens)
{
    traceOutput.clear();
    Tracing::SetTracingFunctions(
        TraceFunc, 
        [](TraceCategory /*category*/) { return false; });
    {
        ScopedTracing trace("MyFile", 123, "MyFunction", [](){ return "This is the begin"; }, [](){ return "This is the end"; });
    }
    EXPECT_EQ("", traceOutput);
}

TEST(ScopedTracingTest, IfEnabledEntryAndExitMessageWritten)
{
    traceOutput.clear();
    Tracing::SetTracingFunctions(
        TraceFunc, 
        [](TraceCategory /*category*/) { return true; });
    {
        ScopedTracing trace("MyFile", 123, "MyFunction", [](){ return "This is the begin"; }, [](){ return "This is the end"; });
    }
    EXPECT_EQ(
        "Enter|MyFile:123|MyFunction|This is the begin\n"
        "Leave|MyFile:123|MyFunction|This is the end\n", traceOutput);
}

TEST(ScopedTracingTest, IfExitFunctionIsNullEmptyStringIsWritten)
{
    traceOutput.clear();
    Tracing::SetTracingFunctions(
        TraceFunc, 
        [](TraceCategory /*category*/) { return true; });
    {
        ScopedTracing trace("MyFile", 123, "MyFunction", nullptr, nullptr);
    }
    EXPECT_EQ(
        "Enter|MyFile:123|MyFunction|\n"
        "Leave|MyFile:123|MyFunction|\n", traceOutput);
}

} // namespace tracing
