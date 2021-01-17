#include "GoogleTest.h"

#include "core/Regex.h"
#include "utility/Error.h"
#include "utility/GenericError.h"
#include "tracing/Logging.h"
#include "tracing/Tracing.h"

namespace tracing {

static const std::string LogRegexTimeStamp = "\\d{4}-\\w{3}-\\d{2}\\s\\d{2}:\\d{2}:\\d{2}\\.\\d{6}\\|";

class LoggingTest : public ::testing::Test
{
public:
    std::string m_logOutput;
    int m_exitCode;
    CategorySet<LogCategory> m_savedLogFilter;

    LoggingTest()
        : m_logOutput()
        , m_exitCode()
        , m_savedLogFilter()
    {}

    void SetUp() override
    {
        Logging::SetLoggingFunction(
            std::bind(&LoggingTest::LogFunc, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, 
                      std::placeholders::_4, std::placeholders::_5, std::placeholders::_6));
        m_savedLogFilter = GetDefaultLogFilter();
        Logging::SetFatalExitFunction([this](int exitCode){ m_exitCode = exitCode; });
    }
    void TearDown() override
    {
        Logging::SetLoggingFunction(nullptr);
        SetDefaultLogFilter(m_savedLogFilter);
        Logging::SetFatalExitFunction(nullptr);    
    }

    void LogFunc(
        osal::EpochTime timestamp,
        LogCategory category,
        const std::string & fileName, 
        int line, 
        const std::string & functionName, 
        const std::string & msg)
    {
        std::ostringstream stream;
        stream << osal::Clock::ToString(timestamp) << "|" << category << "|" << fileName << ":" << line << "|" << functionName << "|" << msg << std::endl;
        m_logOutput += stream.str();
    }
};

TEST_F(LoggingTest, IfLoggingFunctionsSetButNothingEnabledNothingHappens)
{
    SetDefaultLogFilter(LogCategory::None);
    Logging::Log(LogCategory::Trace, "MyFile", 123, "MyFunction", "Hello World");
    Logging::Log(LogCategory::Debug, "MyFile", 123, "MyFunction", "Hello World");
    Logging::Log(LogCategory::Information, "MyFile", 123, "MyFunction", "Hello World");
    Logging::Log(LogCategory::Warning, "MyFile", 123, "MyFunction", "Hello World");
    Logging::Log(LogCategory::Error, "MyFile", 123, "MyFunction", "Hello World");
    Logging::Log(LogCategory::Fatal, "MyFile", 123, "MyFunction", "Hello World");
    EXPECT_EQ("", m_logOutput);
}

TEST_F(LoggingTest, IfLoggingFunctionsSetAndCategoryEnabledLogIsWritten)
{
    SetDefaultLogFilter(LogCategory::All);
    Logging::Log(LogCategory::Debug, "MyFile", 123, "MyFunction", "Hello World");
    EXPECT_TRUE(core::VerifyMatch(m_logOutput, 
        LogRegexTimeStamp + "Debug\\|MyFile\\:123\\|MyFunction\\|Hello World\n"));
}

TEST_F(LoggingTest, IfLogTracingIsEnabledTraceIsWritten)
{
    SetDefaultLogFilter(LogCategory::All);
    std::string traceOutput;
    Tracing::SetTracingFunction(
        [&](osal::EpochTime timestamp,
            TraceCategory category,
            const std::string & fileName, 
            int line, 
            const std::string & functionName, 
            const std::string & msg)
        {
            std::ostringstream stream;
            stream << osal::Clock::ToString(timestamp) << "|" << category << "|" << fileName << ":" << line << "|" << functionName << "|" << msg << std::endl;
            traceOutput += stream.str();
        });
    auto savedTraceFilter = GetDefaultTraceFilter();
    SetDefaultTraceFilter(TraceCategory::Log);
    Logging::Log(LogCategory::Debug, "MyFile", 123, "MyFunction", "Hello World");
    EXPECT_TRUE(core::VerifyMatch(m_logOutput, 
        LogRegexTimeStamp + "Debug\\|MyFile\\:123\\|MyFunction\\|Hello World\n"));
    EXPECT_TRUE(core::VerifyMatch(traceOutput, 
        LogRegexTimeStamp + "Log  \\|MyFile\\:123\\|MyFunction\\|Debug\\|Hello World\n"));
    Tracing::SetTracingFunction(nullptr);
    SetDefaultTraceFilter(savedTraceFilter);
}

TEST_F(LoggingTest, LoggingOnlyForEnabledCategory)
{
    SetDefaultLogFilter(LogCategory::Information);
    Logging::Log(LogCategory::Debug, "MyFile", 123, "MyFunction", "Hello World");
    Logging::Log(LogCategory::Information, "MyFile", 123, "MyFunction", "Hello World");
    EXPECT_TRUE(core::VerifyMatch(m_logOutput, 
        LogRegexTimeStamp + "Info \\|MyFile\\:123\\|MyFunction\\|Hello World\n"));
}

TEST_F(LoggingTest, IfNoLoggingEnableFunctionSetOnlyDefaultCategoriesAreEnabled)
{
    // Assumption: Default filter contains Fatal and Error only
    Logging::Log(LogCategory::Fatal, "MyFile", 123, "MyFunction", "Hello World");
    Logging::Log(LogCategory::Error, "MyFile", 123, "MyFunction", "Hello World");
    Logging::Log(LogCategory::Warning, "MyFile", 123, "MyFunction", "Hello World");
    EXPECT_TRUE(core::VerifyMatch(m_logOutput, 
        LogRegexTimeStamp + "Fatal\\|MyFile\\:123\\|MyFunction\\|Hello World\n" +
        LogRegexTimeStamp + "Error\\|MyFile\\:123\\|MyFunction\\|Hello World\n"));
}

TEST_F(LoggingTest, IfDefaultFilterIsChangedOnlySpecifiedCategoriesAreEnabled)
{
    SetDefaultLogFilter(LogCategory::Fatal);
    Logging::Log(LogCategory::Fatal, "MyFile", 123, "MyFunction", "Hello World");
    Logging::Log(LogCategory::Error, "MyFile", 123, "MyFunction", "Hello World");
    Logging::Log(LogCategory::Warning, "MyFile", 123, "MyFunction", "Hello World");
    EXPECT_TRUE(core::VerifyMatch(m_logOutput, 
        LogRegexTimeStamp + "Fatal\\|MyFile\\:123\\|MyFunction\\|Hello World\n"));
}

TEST_F(LoggingTest, LoggingCategories)
{
    SetDefaultLogFilter(LogCategory::All);
    Logging::Log(LogCategory::Trace, "MyFile", 123, "MyFunction", "Hello World");
    Logging::Log(LogCategory::Debug, "MyFile", 123, "MyFunction", "Hello World");
    Logging::Log(LogCategory::Information, "MyFile", 123, "MyFunction", "Hello World");
    Logging::Log(LogCategory::Warning, "MyFile", 123, "MyFunction", "Hello World");
    Logging::Log(LogCategory::Error, "MyFile", 123, "MyFunction", "Hello World");
    Logging::Log(LogCategory::Fatal, "MyFile", 123, "MyFunction", "Hello World");
    EXPECT_TRUE(core::VerifyMatch(m_logOutput, 
        LogRegexTimeStamp + "Trace\\|MyFile\\:123\\|MyFunction\\|Hello World\n" + 
        LogRegexTimeStamp + "Debug\\|MyFile\\:123\\|MyFunction\\|Hello World\n" + 
        LogRegexTimeStamp + "Info \\|MyFile\\:123\\|MyFunction\\|Hello World\n" + 
        LogRegexTimeStamp + "Warn \\|MyFile\\:123\\|MyFunction\\|Hello World\n" + 
        LogRegexTimeStamp + "Error\\|MyFile\\:123\\|MyFunction\\|Hello World\n" + 
        LogRegexTimeStamp + "Fatal\\|MyFile\\:123\\|MyFunction\\|Hello World\n"));
}

TEST_F(LoggingTest, LoggingWithFormat)
{
    SetDefaultLogFilter(LogCategory::Information);
    Logging::Log(LogCategory::Information, "MyFile", 123, "MyFunction", "{0} {1} (C) {2,4:D}", "Hello", "World", 2020);
    EXPECT_TRUE(core::VerifyMatch(m_logOutput, 
        LogRegexTimeStamp + "Info \\|MyFile\\:123\\|MyFunction\\|Hello World \\(C\\) 2020\n"));
}

TEST_F(LoggingTest, Error)
{
    utility::Error error(2, strerror(2), "Fake error");
    Logging::Error("MyFile", 123, "MyFunction", error);
    EXPECT_TRUE(core::VerifyMatch(m_logOutput, 
        LogRegexTimeStamp + "Error\\|MyFile\\:123\\|MyFunction\\|Error code\\: 2 \\(02\\)\\: No such file or directory Fake error\n"));
}

TEST_F(LoggingTest, ErrorUnknownError)
{
    utility::Error error(-1, "", "Fake error");
    Logging::Error("MyFile", 123, "MyFunction", error);
    EXPECT_TRUE(core::VerifyMatch(m_logOutput, 
        LogRegexTimeStamp + "Error\\|MyFile\\:123\\|MyFunction\\|Error code\\: Unknown\\: Fake error\n"));
}

TEST_F(LoggingTest, Fatal)
{
    utility::Error error(2, strerror(2), "Fake error");
    EXPECT_EQ(0, m_exitCode);
    Logging::Fatal("MyFile", 123, "MyFunction", error);
    EXPECT_TRUE(core::VerifyMatch(m_logOutput, 
        LogRegexTimeStamp + "Fatal\\|MyFile\\:123\\|MyFunction\\|Error code\\: 2 \\(02\\)\\: No such file or directory Fake error\n"));
    EXPECT_EQ(2, m_exitCode);
}

TEST_F(LoggingTest, FatalUnknownError)
{
    utility::Error error(-1, "", "Fake error");
    EXPECT_EQ(0, m_exitCode);
    Logging::Fatal("MyFile", 123, "MyFunction", error);
    EXPECT_TRUE(core::VerifyMatch(m_logOutput, 
        LogRegexTimeStamp + "Fatal\\|MyFile\\:123\\|MyFunction\\|Error code\\: Unknown\\: Fake error\n"));
    EXPECT_EQ(-1, m_exitCode);
}

TEST_F(LoggingTest, FatalGenericError)
{
    utility::GenericError error("Fake error");
    EXPECT_EQ(0, m_exitCode);
    Logging::Fatal("MyFile", 123, "MyFunction", error);
    EXPECT_TRUE(core::VerifyMatch(m_logOutput, 
        LogRegexTimeStamp + "Fatal\\|MyFile\\:123\\|MyFunction\\|Fake error\n"));
    EXPECT_EQ(1, m_exitCode);
}

TEST_F(LoggingTest, ThrowError)
{
    utility::Error error(2, strerror(2), "Fake error");

    try
    {
        Logging::Throw("MyFile", 123, "MyFunction", error);
    }
    catch (std::exception & e)
    {
        m_logOutput = e.what();        
    }
    EXPECT_TRUE(core::VerifyMatch(m_logOutput, 
        LogRegexTimeStamp + "Error MyFile\\:123 \\(MyFunction\\)\\: Fake error\\: Error code\\: 2 \\(02\\)\\: No such file or directory"));
}

TEST_F(LoggingTest, ThrowGenericError)
{
    utility::GenericError error("Fake error");

    try
    {
        Logging::Throw("MyFile", 123, "MyFunction", error);
    }
    catch (std::exception & e)
    {
        m_logOutput = e.what();        
    }
    EXPECT_TRUE(core::VerifyMatch(m_logOutput, 
        LogRegexTimeStamp + "Error MyFile\\:123 \\(MyFunction\\)\\: Fake error"));
}

TEST_F(LoggingTest, LogDebugString)
{
    SetDefaultLogFilter(LogCategory::All);
    LogDebug("MyFile", 123, "MyFunction", "Hello World");
    EXPECT_TRUE(core::VerifyMatch(m_logOutput, 
        LogRegexTimeStamp + "Debug\\|MyFile\\:123\\|MyFunction\\|Hello World\n"));
}

TEST_F(LoggingTest, LogDebugFormatted)
{
    SetDefaultLogFilter(LogCategory::All);
    LogDebug("MyFile", 123, "MyFunction", "{0} {1}", "Hello", "World");
    EXPECT_TRUE(core::VerifyMatch(m_logOutput, 
        LogRegexTimeStamp + "Debug\\|MyFile\\:123\\|MyFunction\\|Hello World\n"));
}

TEST_F(LoggingTest, LogInfoString)
{
    SetDefaultLogFilter(LogCategory::All);
    LogInfo("MyFile", 123, "MyFunction", "Hello World");
    EXPECT_TRUE(core::VerifyMatch(m_logOutput, 
        LogRegexTimeStamp + "Info \\|MyFile\\:123\\|MyFunction\\|Hello World\n"));
}

TEST_F(LoggingTest, LogInfoFormatted)
{
    SetDefaultLogFilter(LogCategory::All);
    LogInfo("MyFile", 123, "MyFunction", "{0} {1}", "Hello", "World");
    EXPECT_TRUE(core::VerifyMatch(m_logOutput, 
        LogRegexTimeStamp + "Info \\|MyFile\\:123\\|MyFunction\\|Hello World\n"));
}

TEST_F(LoggingTest, LogWarningString)
{
    SetDefaultLogFilter(LogCategory::All);
    LogWarning("MyFile", 123, "MyFunction", "Hello World");
    EXPECT_TRUE(core::VerifyMatch(m_logOutput, 
        LogRegexTimeStamp + "Warn \\|MyFile\\:123\\|MyFunction\\|Hello World\n"));
}

TEST_F(LoggingTest, LogWarningFormatted)
{
    SetDefaultLogFilter(LogCategory::All);
    LogWarning("MyFile", 123, "MyFunction", "{0} {1}", "Hello", "World");
    EXPECT_TRUE(core::VerifyMatch(m_logOutput, 
        LogRegexTimeStamp + "Warn \\|MyFile\\:123\\|MyFunction\\|Hello World\n"));
}

TEST_F(LoggingTest, LogErrorString)
{
    SetDefaultLogFilter(LogCategory::All);
    LogError("MyFile", 123, "MyFunction", "Hello World");
    EXPECT_TRUE(core::VerifyMatch(m_logOutput, 
        LogRegexTimeStamp + "Error\\|MyFile\\:123\\|MyFunction\\|Hello World\n"));
}

TEST_F(LoggingTest, LogErrorFormatted)
{
    SetDefaultLogFilter(LogCategory::All);
    LogError("MyFile", 123, "MyFunction", "{0} {1}", "Hello", "World");
    EXPECT_TRUE(core::VerifyMatch(m_logOutput, 
        LogRegexTimeStamp + "Error\\|MyFile\\:123\\|MyFunction\\|Hello World\n"));
}

TEST_F(LoggingTest, LogFatalString)
{
    SetDefaultLogFilter(LogCategory::All);
    LogFatal("MyFile", 123, "MyFunction", "Hello World");
    EXPECT_TRUE(core::VerifyMatch(m_logOutput, 
        LogRegexTimeStamp + "Fatal\\|MyFile\\:123\\|MyFunction\\|Hello World\n"));
}

TEST_F(LoggingTest, LogFatalFormatted)
{
    SetDefaultLogFilter(LogCategory::All);
    LogFatal("MyFile", 123, "MyFunction", "{0} {1}", "Hello", "World");
    EXPECT_TRUE(core::VerifyMatch(m_logOutput, 
        LogRegexTimeStamp + "Fatal\\|MyFile\\:123\\|MyFunction\\|Hello World\n"));
}

TEST_F(LoggingTest, LogToConsole)
{
    SetDefaultLogFilter(LogCategory::All);
    auto savedTraceFilter = GetDefaultTraceFilter();
    SetDefaultTraceFilter(TraceCategory::All);

    LogError("MyFile", 123, "MyFunction", "{0} {1}", "Hello", "World");
    SetDefaultTraceFilter(savedTraceFilter);
}

} // namespace tracing
