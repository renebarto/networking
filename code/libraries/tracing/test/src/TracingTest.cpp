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
    Tracing::Trace(TraceCategory::Startup, "MyFile", 123, "MyFunction", "Hello World");
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
    Tracing::Trace(TraceCategory::Startup, "MyFile", 123, "MyFunction", "Hello World");
    EXPECT_EQ("Start|MyFile:123|MyFunction|Hello World\n", traceOutput);
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
        [](TraceCategory category) { return category == TraceCategory::Startup; });
    Tracing::Trace(TraceCategory::Startup, "MyFile", 123, "MyFunction", "Hello World");
    Tracing::Trace(TraceCategory::Shutdown, "MyFile", 123, "MyFunction", "Hello World");
    EXPECT_EQ("Start|MyFile:123|MyFunction|Hello World\n", traceOutput);
}

TEST(TracingTest, TracingCategories)
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

    Tracing::Trace(TraceCategory::FunctionEnter, "MyFile", 123, "MyFunction", "Hello World");
    Tracing::Trace(TraceCategory::FunctionLeave, "MyFile", 123, "MyFunction", "Hello World");
    Tracing::Trace(TraceCategory::Startup, "MyFile", 123, "MyFunction", "Hello World");
    Tracing::Trace(TraceCategory::Shutdown, "MyFile", 123, "MyFunction", "Hello World");
    Tracing::Trace(TraceCategory::Log, "MyFile", 123, "MyFunction", "Hello World");
    Tracing::Trace(TraceCategory::Message, "MyFile", 123, "MyFunction", "Hello World");
    Tracing::Trace(TraceCategory::Data, "MyFile", 123, "MyFunction", "Hello World");
    EXPECT_EQ(
        "Enter|MyFile:123|MyFunction|Hello World\n"
        "Leave|MyFile:123|MyFunction|Hello World\n"
        "Start|MyFile:123|MyFunction|Hello World\n"
        "Shtdn|MyFile:123|MyFunction|Hello World\n"
        "Log  |MyFile:123|MyFunction|Hello World\n"
        "Messg|MyFile:123|MyFunction|Hello World\n"
        "Data |MyFile:123|MyFunction|Hello World\n", traceOutput);
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
        [](TraceCategory category) { return category == TraceCategory::Message; });
    Tracing::Trace(TraceCategory::Message, "MyFile", 123, "MyFunction", "{0} {1} (C) {2,4:D}", "Hello", "World", 2020);
    EXPECT_EQ("Messg|MyFile:123|MyFunction|Hello World (C) 2020\n", traceOutput);
}

TEST(TracingTest, TraceFuncEnterString)
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

    TraceFuncEnter("MyFile", 123, "MyFunction", "Hello World");
    EXPECT_EQ(
        "Enter|MyFile:123|MyFunction|Hello World\n", traceOutput);
}

TEST(TracingTest, TraceFuncEnterFormatted)
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

    TraceFuncEnter("MyFile", 123, "MyFunction", "{0} {1}", "Hello", "World");
    EXPECT_EQ(
        "Enter|MyFile:123|MyFunction|Hello World\n", traceOutput);
}

TEST(TracingTest, TraceFuncLeaveString)
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

    TraceFuncLeave("MyFile", 123, "MyFunction", "Hello World");
    EXPECT_EQ(
        "Leave|MyFile:123|MyFunction|Hello World\n", traceOutput);
}

TEST(TracingTest, TraceFuncLeaveFormatted)
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

    TraceFuncLeave("MyFile", 123, "MyFunction", "{0} {1}", "Hello", "World");
    EXPECT_EQ(
        "Leave|MyFile:123|MyFunction|Hello World\n", traceOutput);
}

TEST(TracingTest, TraceStartup)
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

    TraceStartup("MyFile", 123, "MyFunction", "Hello World");
    EXPECT_EQ(
        "Start|MyFile:123|MyFunction|Hello World\n", traceOutput);
}

TEST(TracingTest, TraceShutdown)
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

    TraceShutdown("MyFile", 123, "MyFunction", "Hello World");
    EXPECT_EQ(
        "Shtdn|MyFile:123|MyFunction|Hello World\n", traceOutput);
}

TEST(TracingTest, TraceToConsole)
{
    Tracing::SetTracingFunctions(
        nullptr, 
        [](TraceCategory /*category*/) { return true; });

    TraceStartup("MyFile", 123, "MyFunction", "Starting");
    TraceShutdown("MyFile", 123, "MyFunction", "Stopping");
}

} // namespace tracing
