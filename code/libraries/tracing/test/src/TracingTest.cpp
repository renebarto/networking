#include "GoogleTest.h"

#include "tracing/Tracing.h"

namespace tracing {

TEST(TracingTest, IfTracingFunctionsSetButNothingEnabledNothingHappens)
{
    std::string traceOutput;
    Tracing::SetTracingFunctions(
        [&](TraceCategory category,
            const std::string & fileName, 
            int line, 
            const std::string & functionName, 
            const std::string & msg)
        {
            std::ostringstream stream;
            stream << category << "|" << fileName << ":" << line << "|" << functionName << "|" << msg << std::endl;
            traceOutput = stream.str();
        }, 
        [](TraceCategory /*category*/) { return false; });
    Tracing::Trace(TraceCategory::Debug, "MyFile", 123, "MyFunction", "Hello World");
    EXPECT_EQ("", traceOutput);
}

TEST(TracingTest, IfTracingFunctionsSetAndCategoryEnabledTraceIsWritten)
{
    std::string traceOutput;
    Tracing::SetTracingFunctions(
        [&](TraceCategory category,
            const std::string & fileName, 
            int line, 
            const std::string & functionName, 
            const std::string & msg)
        {
            std::ostringstream stream;
            stream << category << "|" << fileName << ":" << line << "|" << functionName << "|" << msg << std::endl;
            traceOutput += stream.str();
        }, 
        [](TraceCategory /*category*/) { return true; });
    Tracing::Trace(TraceCategory::Debug, "MyFile", 123, "MyFunction", "Hello World");
    EXPECT_EQ("Dbg|MyFile:123|MyFunction|Hello World\n", traceOutput);
}

TEST(TracingTest, TracingOnlyForEnabledCategory)
{
    std::string traceOutput;
    Tracing::SetTracingFunctions(
        [&](TraceCategory category,
            const std::string & fileName, 
            int line, 
            const std::string & functionName, 
            const std::string & msg)
        {
            std::ostringstream stream;
            stream << category << "|" << fileName << ":" << line << "|" << functionName << "|" << msg << std::endl;
            traceOutput = stream.str();
        }, 
        [](TraceCategory category) { return category == TraceCategory::Information; });
    Tracing::Trace(TraceCategory::Debug, "MyFile", 123, "MyFunction", "Hello World");
    Tracing::Trace(TraceCategory::Information, "MyFile", 123, "MyFunction", "Hello World");
    EXPECT_EQ("Info|MyFile:123|MyFunction|Hello World\n", traceOutput);
}

TEST(TracingTest, TracingWithFormat)
{
    std::string traceOutput;
    Tracing::SetTracingFunctions(
        [&](TraceCategory category,
            const std::string & fileName, 
            int line, 
            const std::string & functionName, 
            const std::string & msg)
        {
            std::ostringstream stream;
            stream << category << "|" << fileName << ":" << line << "|" << functionName << "|" << msg << std::endl;
            traceOutput = stream.str();
        }, 
        [](TraceCategory category) { return category == TraceCategory::Information; });
    Tracing::Trace(TraceCategory::Information, "MyFile", 123, "MyFunction", "{0} {1} (C) {2,4:D}", "Hello", "World", 2020);
    EXPECT_EQ("Info|MyFile:123|MyFunction|Hello World (C) 2020\n", traceOutput);
}

} // namespace tracing
