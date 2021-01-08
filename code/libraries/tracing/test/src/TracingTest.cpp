#include "GoogleTest.h"

#include "utility/Error.h"
#include "utility/GenericError.h"
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
    EXPECT_EQ("Debug|MyFile:123|MyFunction|Hello World\n", traceOutput);
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

    Tracing::Trace(TraceCategory::FunctionBegin, "MyFile", 123, "MyFunction", "Hello World");
    Tracing::Trace(TraceCategory::FunctionEnd, "MyFile", 123, "MyFunction", "Hello World");
    Tracing::Trace(TraceCategory::Information, "MyFile", 123, "MyFunction", "Hello World");
    Tracing::Trace(TraceCategory::Log, "MyFile", 123, "MyFunction", "Hello World");
    Tracing::Trace(TraceCategory::Startup, "MyFile", 123, "MyFunction", "Hello World");
    Tracing::Trace(TraceCategory::Shutdown, "MyFile", 123, "MyFunction", "Hello World");
    Tracing::Trace(TraceCategory::Debug, "MyFile", 123, "MyFunction", "Hello World");
    Tracing::Trace(TraceCategory::Error, "MyFile", 123, "MyFunction", "Hello World");
    EXPECT_EQ(
        "FuncBeg|MyFile:123|MyFunction|Hello World\n"
        "FuncEnd|MyFile:123|MyFunction|Hello World\n"
        "Info|MyFile:123|MyFunction|Hello World\n"
        "Log|MyFile:123|MyFunction|Hello World\n"
        "Startup|MyFile:123|MyFunction|Hello World\n"
        "Shutdown|MyFile:123|MyFunction|Hello World\n"
        "Debug|MyFile:123|MyFunction|Hello World\n"
        "Error|MyFile:123|MyFunction|Hello World\n", traceOutput);
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

TEST(TracingTest, TracerTrace)
{
    utility::Error error(2, strerror(2), "Fake error");
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

    Tracer::Trace("MyFile", 123, "MyFunction", error);
    EXPECT_EQ(
        "Error|MyFile:123|MyFunction|Error code: 2 (02): No such file or directory Fake error\n", traceOutput);
}

TEST(TracingTest, TracerTraceUnknownError)
{
    utility::Error error(-1, "", "Fake error");
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

    Tracer::Trace("MyFile", 123, "MyFunction", error);
    EXPECT_EQ(
        "Error|MyFile:123|MyFunction|Error code: Unknown: Fake error\n", traceOutput);
}

TEST(TracingTest, TracerThrowError)
{
    utility::Error error(2, strerror(2), "Fake error");
    std::string traceOutput;

    try
    {
        Tracer::Throw("MyFile", 123, "MyFunction", error);
    }
    catch (std::exception & e)
    {
        traceOutput = e.what();        
    }
    EXPECT_EQ(
        "Error MyFile:123(MyFunction): Fake error: Error code: 2 (02): No such file or directory", traceOutput);
}

TEST(TracingTest, TracerThrowGenericError)
{
    utility::GenericError error("Fake error");
    std::string traceOutput;

    try
    {
        Tracer::Throw("MyFile", 123, "MyFunction", error);
    }
    catch (std::exception & e)
    {
        traceOutput = e.what();        
    }
    EXPECT_EQ(
        "Error MyFile:123(MyFunction): Fake error", traceOutput);
}

TEST(TracingTest, TraceDebugString)
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

    TraceDebug("MyFile", 123, "MyFunction", "Hello World");
    EXPECT_EQ(
        "Debug|MyFile:123|MyFunction|Hello World\n", traceOutput);
}

TEST(TracingTest, TraceDebugFormatted)
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

    TraceDebug("MyFile", 123, "MyFunction", "{0} {1}", "Hello", "World");
    EXPECT_EQ(
        "Debug|MyFile:123|MyFunction|Hello World\n", traceOutput);
}

TEST(TracingTest, TraceInfoString)
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

    TraceInfo("MyFile", 123, "MyFunction", "Hello World");
    EXPECT_EQ(
        "Info|MyFile:123|MyFunction|Hello World\n", traceOutput);
}

TEST(TracingTest, TraceInfoFormatted)
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

    TraceInfo("MyFile", 123, "MyFunction", "{0} {1}", "Hello", "World");
    EXPECT_EQ(
        "Info|MyFile:123|MyFunction|Hello World\n", traceOutput);
}

TEST(TracingTest, TraceErrorString)
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

    TraceError("MyFile", 123, "MyFunction", "Hello World");
    EXPECT_EQ(
        "Error|MyFile:123|MyFunction|Hello World\n", traceOutput);
}

TEST(TracingTest, TraceErrorFormatted)
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

    TraceError("MyFile", 123, "MyFunction", "{0} {1}", "Hello", "World");
    EXPECT_EQ(
        "Error|MyFile:123|MyFunction|Hello World\n", traceOutput);
}

TEST(TracingTest, TraceToConsole)
{
    Tracing::SetTracingFunctions(
        nullptr, 
        [](TraceCategory /*category*/) { return true; });

    TraceError("MyFile", 123, "MyFunction", "{0} {1}", "Hello", "World");
}

} // namespace tracing
