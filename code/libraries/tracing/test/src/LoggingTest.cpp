#include "GoogleTest.h"

#include "utility/Error.h"
#include "utility/GenericError.h"
#include "tracing/Logging.h"

namespace tracing {

TEST(LoggingTest, IfLoggingFunctionsSetButNothingEnabledNothingHappens)
{
    std::string traceOutput;
    Logging::SetLoggingFunctions(
        [&](LogCategory category,
            const std::string & fileName, 
            int line, 
            const std::string & functionName, 
            const std::string & msg)
        {
            std::ostringstream stream;
            stream << category << "|" << fileName << ":" << line << "|" << functionName << "|" << msg << std::endl;
            traceOutput = stream.str();
        }, 
        [](LogCategory /*category*/) { return false; });
    Logging::Log(LogCategory::Debug, "MyFile", 123, "MyFunction", "Hello World");
    EXPECT_EQ("", traceOutput);
}

TEST(LoggingTest, IfLoggingFunctionsSetAndCategoryEnabledLogIsWritten)
{
    std::string traceOutput;
    Logging::SetLoggingFunctions(
        [&](LogCategory category,
            const std::string & fileName, 
            int line, 
            const std::string & functionName, 
            const std::string & msg)
        {
            std::ostringstream stream;
            stream << category << "|" << fileName << ":" << line << "|" << functionName << "|" << msg << std::endl;
            traceOutput += stream.str();
        }, 
        [](LogCategory /*category*/) { return true; });
    Logging::Log(LogCategory::Debug, "MyFile", 123, "MyFunction", "Hello World");
    EXPECT_EQ("Debug|MyFile:123|MyFunction|Hello World\n", traceOutput);
}

TEST(LoggingTest, LoggingOnlyForEnabledCategory)
{
    std::string traceOutput;
    Logging::SetLoggingFunctions(
        [&](LogCategory category,
            const std::string & fileName, 
            int line, 
            const std::string & functionName, 
            const std::string & msg)
        {
            std::ostringstream stream;
            stream << category << "|" << fileName << ":" << line << "|" << functionName << "|" << msg << std::endl;
            traceOutput = stream.str();
        }, 
        [](LogCategory category) { return category == LogCategory::Information; });
    Logging::Log(LogCategory::Debug, "MyFile", 123, "MyFunction", "Hello World");
    Logging::Log(LogCategory::Information, "MyFile", 123, "MyFunction", "Hello World");
    EXPECT_EQ("Info |MyFile:123|MyFunction|Hello World\n", traceOutput);
}

TEST(LoggingTest, LoggingCategories)
{
    std::string traceOutput;
    Logging::SetLoggingFunctions(
        [&](LogCategory category,
            const std::string & fileName, 
            int line, 
            const std::string & functionName, 
            const std::string & msg)
        {
            std::ostringstream stream;
            stream << category << "|" << fileName << ":" << line << "|" << functionName << "|" << msg << std::endl;
            traceOutput += stream.str();
        }, 
        [](LogCategory /*category*/) { return true; });

    Logging::Log(LogCategory::Trace, "MyFile", 123, "MyFunction", "Hello World");
    Logging::Log(LogCategory::Debug, "MyFile", 123, "MyFunction", "Hello World");
    Logging::Log(LogCategory::Information, "MyFile", 123, "MyFunction", "Hello World");
    Logging::Log(LogCategory::Warning, "MyFile", 123, "MyFunction", "Hello World");
    Logging::Log(LogCategory::Error, "MyFile", 123, "MyFunction", "Hello World");
    Logging::Log(LogCategory::Fatal, "MyFile", 123, "MyFunction", "Hello World");
    EXPECT_EQ(
        "Trace|MyFile:123|MyFunction|Hello World\n"
        "Debug|MyFile:123|MyFunction|Hello World\n"
        "Info |MyFile:123|MyFunction|Hello World\n"
        "Warn |MyFile:123|MyFunction|Hello World\n"
        "Error|MyFile:123|MyFunction|Hello World\n"
        "Fatal|MyFile:123|MyFunction|Hello World\n", traceOutput);
}

TEST(LoggingTest, LoggingWithFormat)
{
    std::string traceOutput;
    Logging::SetLoggingFunctions(
        [&](LogCategory category,
            const std::string & fileName, 
            int line, 
            const std::string & functionName, 
            const std::string & msg)
        {
            std::ostringstream stream;
            stream << category << "|" << fileName << ":" << line << "|" << functionName << "|" << msg << std::endl;
            traceOutput = stream.str();
        }, 
        [](LogCategory category) { return category == LogCategory::Information; });
    Logging::Log(LogCategory::Information, "MyFile", 123, "MyFunction", "{0} {1} (C) {2,4:D}", "Hello", "World", 2020);
    EXPECT_EQ("Info |MyFile:123|MyFunction|Hello World (C) 2020\n", traceOutput);
}

TEST(LoggingTest, Error)
{
    utility::Error error(2, strerror(2), "Fake error");
    std::string traceOutput;
    Logging::SetLoggingFunctions(
        [&](LogCategory category,
            const std::string & fileName, 
            int line, 
            const std::string & functionName, 
            const std::string & msg)
        {
            std::ostringstream stream;
            stream << category << "|" << fileName << ":" << line << "|" << functionName << "|" << msg << std::endl;
            traceOutput += stream.str();
        }, 
        [](LogCategory /*category*/) { return true; });

    Logging::Error("MyFile", 123, "MyFunction", error);
    EXPECT_EQ(
        "Error|MyFile:123|MyFunction|Error code: 2 (02): No such file or directory Fake error\n", traceOutput);
}

TEST(LoggingTest, ErrorUnknownError)
{
    utility::Error error(-1, "", "Fake error");
    std::string traceOutput;
    Logging::SetLoggingFunctions(
        [&](LogCategory category,
            const std::string & fileName, 
            int line, 
            const std::string & functionName, 
            const std::string & msg)
        {
            std::ostringstream stream;
            stream << category << "|" << fileName << ":" << line << "|" << functionName << "|" << msg << std::endl;
            traceOutput += stream.str();
        }, 
        [](LogCategory /*category*/) { return true; });

    Logging::Error("MyFile", 123, "MyFunction", error);
    EXPECT_EQ(
        "Error|MyFile:123|MyFunction|Error code: Unknown: Fake error\n", traceOutput);
}

class LoggingFatalTest : public ::testing::Test
{
public:
    int m_exitCode;

    LoggingFatalTest()
        : m_exitCode()
    {}

    void SetUp() override
    {
        Logging::SetFatalExitFunction([this](int exitCode){ m_exitCode = exitCode; });
    }
    void TearDown() override
    {
        Logging::SetFatalExitFunction(nullptr);    
    }
};

TEST_F(LoggingFatalTest, Fatal)
{
    utility::Error error(2, strerror(2), "Fake error");
    std::string traceOutput;
    Logging::SetLoggingFunctions(
        [&](LogCategory category,
            const std::string & fileName, 
            int line, 
            const std::string & functionName, 
            const std::string & msg)
        {
            std::ostringstream stream;
            stream << category << "|" << fileName << ":" << line << "|" << functionName << "|" << msg << std::endl;
            traceOutput += stream.str();
        }, 
        [](LogCategory /*category*/) { return true; });

    EXPECT_EQ(0, m_exitCode);
    Logging::Fatal("MyFile", 123, "MyFunction", error);
    EXPECT_EQ(
        "Fatal|MyFile:123|MyFunction|Error code: 2 (02): No such file or directory Fake error\n", traceOutput);
    EXPECT_EQ(2, m_exitCode);
}

TEST_F(LoggingFatalTest, FatalUnknownError)
{
    utility::Error error(-1, "", "Fake error");
    std::string traceOutput;
    Logging::SetLoggingFunctions(
        [&](LogCategory category,
            const std::string & fileName, 
            int line, 
            const std::string & functionName, 
            const std::string & msg)
        {
            std::ostringstream stream;
            stream << category << "|" << fileName << ":" << line << "|" << functionName << "|" << msg << std::endl;
            traceOutput += stream.str();
        }, 
        [](LogCategory /*category*/) { return true; });

    EXPECT_EQ(0, m_exitCode);
    Logging::Fatal("MyFile", 123, "MyFunction", error);
    EXPECT_EQ(
        "Fatal|MyFile:123|MyFunction|Error code: Unknown: Fake error\n", traceOutput);
    EXPECT_EQ(-1, m_exitCode);
}

TEST_F(LoggingFatalTest, FatalGenericError)
{
    utility::GenericError error("Fake error");
    std::string traceOutput;
    Logging::SetLoggingFunctions(
        [&](LogCategory category,
            const std::string & fileName, 
            int line, 
            const std::string & functionName, 
            const std::string & msg)
        {
            std::ostringstream stream;
            stream << category << "|" << fileName << ":" << line << "|" << functionName << "|" << msg << std::endl;
            traceOutput += stream.str();
        }, 
        [](LogCategory /*category*/) { return true; });

    EXPECT_EQ(0, m_exitCode);
    Logging::Fatal("MyFile", 123, "MyFunction", error);
    EXPECT_EQ(
        "Fatal|MyFile:123|MyFunction|Fake error\n", traceOutput);
    EXPECT_EQ(1, m_exitCode);
}

TEST(LoggingTest, ThrowError)
{
    utility::Error error(2, strerror(2), "Fake error");
    std::string traceOutput;

    try
    {
        Logging::Throw("MyFile", 123, "MyFunction", error);
    }
    catch (std::exception & e)
    {
        traceOutput = e.what();        
    }
    EXPECT_EQ(
        "Error MyFile:123(MyFunction): Fake error: Error code: 2 (02): No such file or directory", traceOutput);
}

TEST(LoggingTest, LogrThrowGenericError)
{
    utility::GenericError error("Fake error");
    std::string traceOutput;

    try
    {
        Logging::Throw("MyFile", 123, "MyFunction", error);
    }
    catch (std::exception & e)
    {
        traceOutput = e.what();        
    }
    EXPECT_EQ(
        "Error MyFile:123(MyFunction): Fake error", traceOutput);
}

TEST(LoggingTest, LogDebugString)
{
    std::string traceOutput;
    Logging::SetLoggingFunctions(
        [&](LogCategory category,
            const std::string & fileName, 
            int line, 
            const std::string & functionName, 
            const std::string & msg)
        {
            std::ostringstream stream;
            stream << category << "|" << fileName << ":" << line << "|" << functionName << "|" << msg << std::endl;
            traceOutput += stream.str();
        }, 
        [](LogCategory /*category*/) { return true; });

    LogDebug("MyFile", 123, "MyFunction", "Hello World");
    EXPECT_EQ(
        "Debug|MyFile:123|MyFunction|Hello World\n", traceOutput);
}

TEST(LoggingTest, LogDebugFormatted)
{
    std::string traceOutput;
    Logging::SetLoggingFunctions(
        [&](LogCategory category,
            const std::string & fileName, 
            int line, 
            const std::string & functionName, 
            const std::string & msg)
        {
            std::ostringstream stream;
            stream << category << "|" << fileName << ":" << line << "|" << functionName << "|" << msg << std::endl;
            traceOutput += stream.str();
        }, 
        [](LogCategory /*category*/) { return true; });

    LogDebug("MyFile", 123, "MyFunction", "{0} {1}", "Hello", "World");
    EXPECT_EQ(
        "Debug|MyFile:123|MyFunction|Hello World\n", traceOutput);
}

TEST(LoggingTest, LogInfoString)
{
    std::string traceOutput;
    Logging::SetLoggingFunctions(
        [&](LogCategory category,
            const std::string & fileName, 
            int line, 
            const std::string & functionName, 
            const std::string & msg)
        {
            std::ostringstream stream;
            stream << category << "|" << fileName << ":" << line << "|" << functionName << "|" << msg << std::endl;
            traceOutput += stream.str();
        }, 
        [](LogCategory /*category*/) { return true; });

    LogInfo("MyFile", 123, "MyFunction", "Hello World");
    EXPECT_EQ(
        "Info |MyFile:123|MyFunction|Hello World\n", traceOutput);
}

TEST(LoggingTest, LogInfoFormatted)
{
    std::string traceOutput;
    Logging::SetLoggingFunctions(
        [&](LogCategory category,
            const std::string & fileName, 
            int line, 
            const std::string & functionName, 
            const std::string & msg)
        {
            std::ostringstream stream;
            stream << category << "|" << fileName << ":" << line << "|" << functionName << "|" << msg << std::endl;
            traceOutput += stream.str();
        }, 
        [](LogCategory /*category*/) { return true; });

    LogInfo("MyFile", 123, "MyFunction", "{0} {1}", "Hello", "World");
    EXPECT_EQ(
        "Info |MyFile:123|MyFunction|Hello World\n", traceOutput);
}

TEST(LoggingTest, LogWarningString)
{
    std::string traceOutput;
    Logging::SetLoggingFunctions(
        [&](LogCategory category,
            const std::string & fileName, 
            int line, 
            const std::string & functionName, 
            const std::string & msg)
        {
            std::ostringstream stream;
            stream << category << "|" << fileName << ":" << line << "|" << functionName << "|" << msg << std::endl;
            traceOutput += stream.str();
        }, 
        [](LogCategory /*category*/) { return true; });

    LogWarning("MyFile", 123, "MyFunction", "Hello World");
    EXPECT_EQ(
        "Warn |MyFile:123|MyFunction|Hello World\n", traceOutput);
}

TEST(LoggingTest, LogWarningFormatted)
{
    std::string traceOutput;
    Logging::SetLoggingFunctions(
        [&](LogCategory category,
            const std::string & fileName, 
            int line, 
            const std::string & functionName, 
            const std::string & msg)
        {
            std::ostringstream stream;
            stream << category << "|" << fileName << ":" << line << "|" << functionName << "|" << msg << std::endl;
            traceOutput += stream.str();
        }, 
        [](LogCategory /*category*/) { return true; });

    LogWarning("MyFile", 123, "MyFunction", "{0} {1}", "Hello", "World");
    EXPECT_EQ(
        "Warn |MyFile:123|MyFunction|Hello World\n", traceOutput);
}

TEST(LoggingTest, LogErrorString)
{
    std::string traceOutput;
    Logging::SetLoggingFunctions(
        [&](LogCategory category,
            const std::string & fileName, 
            int line, 
            const std::string & functionName, 
            const std::string & msg)
        {
            std::ostringstream stream;
            stream << category << "|" << fileName << ":" << line << "|" << functionName << "|" << msg << std::endl;
            traceOutput += stream.str();
        }, 
        [](LogCategory /*category*/) { return true; });

    LogError("MyFile", 123, "MyFunction", "Hello World");
    EXPECT_EQ(
        "Error|MyFile:123|MyFunction|Hello World\n", traceOutput);
}

TEST(LoggingTest, LogErrorFormatted)
{
    std::string traceOutput;
    Logging::SetLoggingFunctions(
        [&](LogCategory category,
            const std::string & fileName, 
            int line, 
            const std::string & functionName, 
            const std::string & msg)
        {
            std::ostringstream stream;
            stream << category << "|" << fileName << ":" << line << "|" << functionName << "|" << msg << std::endl;
            traceOutput += stream.str();
        }, 
        [](LogCategory /*category*/) { return true; });

    LogError("MyFile", 123, "MyFunction", "{0} {1}", "Hello", "World");
    EXPECT_EQ(
        "Error|MyFile:123|MyFunction|Hello World\n", traceOutput);
}

TEST(LoggingTest, LogFatalString)
{
    std::string traceOutput;
    Logging::SetLoggingFunctions(
        [&](LogCategory category,
            const std::string & fileName, 
            int line, 
            const std::string & functionName, 
            const std::string & msg)
        {
            std::ostringstream stream;
            stream << category << "|" << fileName << ":" << line << "|" << functionName << "|" << msg << std::endl;
            traceOutput += stream.str();
        }, 
        [](LogCategory /*category*/) { return true; });

    LogFatal("MyFile", 123, "MyFunction", "Hello World");
    EXPECT_EQ(
        "Fatal|MyFile:123|MyFunction|Hello World\n", traceOutput);
}

TEST(LoggingTest, LogFatalFormatted)
{
    std::string traceOutput;
    Logging::SetLoggingFunctions(
        [&](LogCategory category,
            const std::string & fileName, 
            int line, 
            const std::string & functionName, 
            const std::string & msg)
        {
            std::ostringstream stream;
            stream << category << "|" << fileName << ":" << line << "|" << functionName << "|" << msg << std::endl;
            traceOutput += stream.str();
        }, 
        [](LogCategory /*category*/) { return true; });

    LogFatal("MyFile", 123, "MyFunction", "{0} {1}", "Hello", "World");
    EXPECT_EQ(
        "Fatal|MyFile:123|MyFunction|Hello World\n", traceOutput);
}

TEST(LoggingTest, LogToConsole)
{
    Logging::SetLoggingFunctions(
        nullptr, 
        [](LogCategory /*category*/) { return true; });

    LogError("MyFile", 123, "MyFunction", "{0} {1}", "Hello", "World");
}

} // namespace tracing
