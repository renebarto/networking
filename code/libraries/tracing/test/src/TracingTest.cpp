#include "GoogleTest.h"

#include "osal/ThreadFunctions.h"
#include "utility/Error.h"
#include "utility/GenericError.h"
#include "utility/Regex.h"
#include "tracing/Tracing.h"

namespace tracing {

static const std::string TraceRegexTimeStamp = "\\d{4}-\\w{3}-\\d{2}\\s\\d{2}:\\d{2}:\\d{2}\\.\\d{6}\\|";

class TracingTest : public ::testing::Test
{
public:
    std::string m_traceOutput;
    int m_exitCode;
    CategorySet<TraceCategory> m_savedTraceFilter;

    TracingTest()
        : m_traceOutput()
        , m_exitCode()
        , m_savedTraceFilter()
    {}

    virtual void SetUp() override
    {
        Tracing::SetTracingFunction(
            std::bind(&TracingTest::TraceFunc, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, 
                      std::placeholders::_4, std::placeholders::_5, std::placeholders::_6));
        m_savedTraceFilter = GetDefaultTraceFilter();
        Tracing::SetFatalExitFunction([this](int exitCode){ m_exitCode = exitCode; });
        osal::SetThreadNameSelf("TracingTest");
    }
    virtual void TearDown() override
    {
        Tracing::SetTracingFunction(nullptr);
        SetDefaultTraceFilter(m_savedTraceFilter);
        Tracing::SetFatalExitFunction(nullptr);    
    }

    void TraceFunc(
        osal::EpochTime timestamp,
        TraceCategory category,
        const std::string & fileName, 
        int line, 
        const std::string & functionName, 
        const std::string & msg)
    {
        std::ostringstream stream;
        stream << osal::Clock::ToString(timestamp) << "|" << category << "|" << fileName << ":" << line << "|" << functionName << "|" << osal::GetThreadNameSelf() << "|" << msg << std::endl;
        m_traceOutput += stream.str();
    }
};

TEST_F(TracingTest, IfTracingFunctionsSetButNothingEnabledNothingHappens)
{
    SetDefaultTraceFilter(TraceCategory::None);
    Tracing::Trace(TraceCategory::StartupShutdown, "MyFile", 123, "MyFunction", "Hello World");
    EXPECT_EQ("", m_traceOutput);
}

TEST_F(TracingTest, IfTracingFunctionsSetAndCategoryEnabledTraceIsWritten)
{
    SetDefaultTraceFilter(TraceCategory::All);
    Tracing::Trace(TraceCategory::StartupShutdown, "MyFile", 123, "MyFunction", "Hello World");
    EXPECT_TRUE(utility::VerifyMatch(m_traceOutput, 
        TraceRegexTimeStamp + "StartupShutdown\\|MyFile\\:123\\|MyFunction\\|TracingTest\\|Hello World\n"));
}

TEST_F(TracingTest, TracingOnlyForEnabledCategory)
{
    SetDefaultTraceFilter(TraceCategory::StartupShutdown);
    Tracing::Trace(TraceCategory::StartupShutdown, "MyFile", 123, "MyFunction", "Hello World");
    EXPECT_TRUE(utility::VerifyMatch(m_traceOutput, 
        TraceRegexTimeStamp + "StartupShutdown\\|MyFile\\:123\\|MyFunction\\|TracingTest\\|Hello World\n"));
}

TEST_F(TracingTest, IfNoTracingEnableFunctionSetOnlyDefaultCategoriesAreEnabled)
{
    // Assumption: default only StartupShutdown enabled
    Tracing::Trace(TraceCategory::StartupShutdown, "MyFile", 123, "MyFunction", "Hello World");
    Tracing::Trace(TraceCategory::Data, "MyFile", 123, "MyFunction", "Hello World");
    EXPECT_TRUE(utility::VerifyMatch(m_traceOutput, 
        TraceRegexTimeStamp + "StartupShutdown\\|MyFile\\:123\\|MyFunction\\|TracingTest\\|Hello World\n"));
}

TEST_F(TracingTest, IfDefaultFilterIsChangedOnlySpecifiedCategoriesAreEnabled)
{
    SetDefaultTraceFilter(TraceCategory::StartupShutdown);
    Tracing::Trace(TraceCategory::StartupShutdown, "MyFile", 123, "MyFunction", "Hello World");
    Tracing::Trace(TraceCategory::Data, "MyFile", 123, "MyFunction", "Hello World");
    EXPECT_TRUE(utility::VerifyMatch(m_traceOutput, 
        TraceRegexTimeStamp + "StartupShutdown\\|MyFile\\:123\\|MyFunction\\|TracingTest\\|Hello World\n"));
}

TEST_F(TracingTest, TracingCategories)
{
    SetDefaultTraceFilter(TraceCategory::All);
    Tracing::Trace(TraceCategory::FunctionEnter, "MyFile", 123, "MyFunction", "Hello World");
    Tracing::Trace(TraceCategory::FunctionLeave, "MyFile", 123, "MyFunction", "Hello World");
    Tracing::Trace(TraceCategory::StartupShutdown, "MyFile", 123, "MyFunction", "Hello World");
    Tracing::Trace(TraceCategory::Log, "MyFile", 123, "MyFunction", "Hello World");
    Tracing::Trace(TraceCategory::Information, "MyFile", 123, "MyFunction", "Hello World");
    Tracing::Trace(TraceCategory::Data, "MyFile", 123, "MyFunction", "Hello World");
    EXPECT_TRUE(utility::VerifyMatch(m_traceOutput, 
        TraceRegexTimeStamp + "Enter\\|MyFile\\:123\\|MyFunction\\|TracingTest\\|Hello World\n" + 
        TraceRegexTimeStamp + "Leave\\|MyFile\\:123\\|MyFunction\\|TracingTest\\|Hello World\n" + 
        TraceRegexTimeStamp + "StartupShutdown\\|MyFile\\:123\\|MyFunction\\|TracingTest\\|Hello World\n" + 
        TraceRegexTimeStamp + "Log\\|MyFile\\:123\\|MyFunction\\|TracingTest\\|Hello World\n" + 
        TraceRegexTimeStamp + "Info\\|MyFile\\:123\\|MyFunction\\|TracingTest\\|Hello World\n" + 
        TraceRegexTimeStamp + "Data\\|MyFile\\:123\\|MyFunction\\|TracingTest\\|Hello World\n"));
}

TEST_F(TracingTest, TracingWithFormat)
{
    SetDefaultTraceFilter(TraceCategory::Information);
    Tracing::Trace(TraceCategory::Information, "MyFile", 123, "MyFunction", "{0} {1} (C) {2,4:D}", "Hello", "World", 2020);
    EXPECT_TRUE(utility::VerifyMatch(m_traceOutput, 
        TraceRegexTimeStamp + "Info\\|MyFile\\:123\\|MyFunction\\|TracingTest\\|Hello World \\(C\\) 2020\n"));
}

TEST_F(TracingTest, TraceFuncEnterString)
{
    SetDefaultTraceFilter(TraceCategory::All);
    TraceFuncEnter("MyFile", 123, "MyFunction", "Hello World");
    EXPECT_TRUE(utility::VerifyMatch(m_traceOutput, 
        TraceRegexTimeStamp + "Enter\\|MyFile\\:123\\|MyFunction\\|TracingTest\\|Hello World\n"));
}

TEST_F(TracingTest, TraceFuncEnterFormatted)
{
    SetDefaultTraceFilter(TraceCategory::All);
    TraceFuncEnter("MyFile", 123, "MyFunction", "{0} {1}", "Hello", "World");
    EXPECT_TRUE(utility::VerifyMatch(m_traceOutput, 
        TraceRegexTimeStamp + "Enter\\|MyFile\\:123\\|MyFunction\\|TracingTest\\|Hello World\n"));
}

TEST_F(TracingTest, TraceFuncLeaveString)
{
    SetDefaultTraceFilter(TraceCategory::All);
    TraceFuncLeave("MyFile", 123, "MyFunction", "Hello World");
    EXPECT_TRUE(utility::VerifyMatch(m_traceOutput, 
        TraceRegexTimeStamp + "Leave\\|MyFile\\:123\\|MyFunction\\|TracingTest\\|Hello World\n"));
}

TEST_F(TracingTest, TraceFuncLeaveFormatted)
{
    SetDefaultTraceFilter(TraceCategory::All);
    TraceFuncLeave("MyFile", 123, "MyFunction", "{0} {1}", "Hello", "World");
    EXPECT_TRUE(utility::VerifyMatch(m_traceOutput, 
        TraceRegexTimeStamp + "Leave\\|MyFile\\:123\\|MyFunction\\|TracingTest\\|Hello World\n"));
}

TEST_F(TracingTest, TraceStartup)
{
    SetDefaultTraceFilter(TraceCategory::All);
    TraceStartup("MyFile", 123, "MyFunction", "Hello World");
    EXPECT_TRUE(utility::VerifyMatch(m_traceOutput, 
        TraceRegexTimeStamp + "StartupShutdown\\|MyFile\\:123\\|MyFunction\\|TracingTest\\|Hello World\n"));
}

TEST_F(TracingTest, TraceShutdown)
{
    SetDefaultTraceFilter(TraceCategory::All);
    TraceShutdown("MyFile", 123, "MyFunction", "Hello World");
    EXPECT_TRUE(utility::VerifyMatch(m_traceOutput, 
        TraceRegexTimeStamp + "Shtdn\\|MyFile\\:123\\|MyFunction\\|TracingTest\\|Hello World\n"));
}

TEST_F(TracingTest, TraceInfoString)
{
    SetDefaultTraceFilter(TraceCategory::All);
    TraceInfo("MyFile", 123, "MyFunction", "Hello World");
    EXPECT_TRUE(utility::VerifyMatch(m_traceOutput, 
        TraceRegexTimeStamp + "Info\\|MyFile\\:123\\|MyFunction\\|TracingTest\\|Hello World\n"));
}

TEST_F(TracingTest, TraceInfoFormatted)
{
    SetDefaultTraceFilter(TraceCategory::All);
    TraceInfo("MyFile", 123, "MyFunction", "{0} {1}", "Hello", "World");
    EXPECT_TRUE(utility::VerifyMatch(m_traceOutput, 
        TraceRegexTimeStamp + "Info\\|MyFile\\:123\\|MyFunction\\|TracingTest\\|Hello World\n"));
}

TEST_F(TracingTest, TraceDataString)
{
    SetDefaultTraceFilter(TraceCategory::All);
    TraceData("MyFile", 123, "MyFunction", "Hello World");
    EXPECT_TRUE(utility::VerifyMatch(m_traceOutput, 
        TraceRegexTimeStamp + "Data\\|MyFile\\:123\\|MyFunction\\|TracingTest\\|Hello World\n"));
}

TEST_F(TracingTest, TraceDataFormatted)
{
    SetDefaultTraceFilter(TraceCategory::All);
    TraceData("MyFile", 123, "MyFunction", "{0} {1}", "Hello", "World");
    EXPECT_TRUE(utility::VerifyMatch(m_traceOutput, 
        TraceRegexTimeStamp + "Data\\|MyFile\\:123\\|MyFunction\\|TracingTest\\|Hello World\n"));
}

TEST_F(TracingTest, TraceDebugString)
{
    SetDefaultTraceFilter(TraceCategory::All);
    TraceDebug("MyFile", 123, "MyFunction", "Hello World");
    EXPECT_TRUE(utility::VerifyMatch(m_traceOutput, 
        TraceRegexTimeStamp + "Debug\\|MyFile\\:123\\|MyFunction\\|TracingTest\\|Hello World\n"));
}

TEST_F(TracingTest, TraceDebugFormatted)
{
    SetDefaultTraceFilter(TraceCategory::All);
    TraceDebug("MyFile", 123, "MyFunction", "{0} {1}", "Hello", "World");
    EXPECT_TRUE(utility::VerifyMatch(m_traceOutput, 
        TraceRegexTimeStamp + "Debug\\|MyFile\\:123\\|MyFunction\\|TracingTest\\|Hello World\n"));
}

TEST_F(TracingTest, TraceToConsole)
{
    SetDefaultTraceFilter(TraceCategory::All);
    TraceStartup("MyFile", 123, "MyFunction", "Starting");
    TraceShutdown("MyFile", 123, "MyFunction", "Stopping");
}

TEST_F(TracingTest, Error)
{
    utility::Error error(2, strerror(2), "Fake error");
    Tracing::Error("MyFile", 123, "MyFunction", error);
    EXPECT_TRUE(utility::VerifyMatch(m_traceOutput, 
        TraceRegexTimeStamp + "Log\\|MyFile\\:123\\|MyFunction\\|TracingTest\\|Error code\\: 2 \\(02\\)\\: No such file or directory Fake error\n"));
}

TEST_F(TracingTest, ErrorUnknownError)
{
    utility::Error error(-1, "", "Fake error");
    Tracing::Error("MyFile", 123, "MyFunction", error);
    EXPECT_TRUE(utility::VerifyMatch(m_traceOutput, 
        TraceRegexTimeStamp + "Log\\|MyFile\\:123\\|MyFunction\\|TracingTest\\|Error code\\: Unknown\\: Fake error\n"));
}

TEST_F(TracingTest, Fatal)
{
    utility::Error error(2, strerror(2), "Fake error");
    EXPECT_EQ(0, m_exitCode);
    Tracing::Fatal("MyFile", 123, "MyFunction", error);
    EXPECT_TRUE(utility::VerifyMatch(m_traceOutput, 
        TraceRegexTimeStamp + "Log\\|MyFile\\:123\\|MyFunction\\|TracingTest\\|Error code\\: 2 \\(02\\)\\: No such file or directory Fake error\n"));
    EXPECT_EQ(2, m_exitCode);
}

TEST_F(TracingTest, FatalUnknownError)
{
    utility::Error error(-1, "", "Fake error");
    EXPECT_EQ(0, m_exitCode);
    Tracing::Fatal("MyFile", 123, "MyFunction", error);
    EXPECT_TRUE(utility::VerifyMatch(m_traceOutput, 
        TraceRegexTimeStamp + "Log\\|MyFile\\:123\\|MyFunction\\|TracingTest\\|Error code\\: Unknown\\: Fake error\n"));
    EXPECT_EQ(-1, m_exitCode);
}

TEST_F(TracingTest, FatalGenericError)
{
    utility::GenericError error("Fake error");
    EXPECT_EQ(0, m_exitCode);
    Tracing::Fatal("MyFile", 123, "MyFunction", error);
    EXPECT_TRUE(utility::VerifyMatch(m_traceOutput, 
        TraceRegexTimeStamp + "Log\\|MyFile\\:123\\|MyFunction\\|TracingTest\\|Fake error\n"));
    EXPECT_EQ(1, m_exitCode);
}

TEST_F(TracingTest, ThrowError)
{
    utility::Error error(2, strerror(2), "Fake error");

    try
    {
        Tracing::Throw("MyFile", 123, "MyFunction", error);
    }
    catch (std::exception & e)
    {
        m_traceOutput = e.what();        
    }
    EXPECT_TRUE(utility::VerifyMatch(m_traceOutput, 
        TraceRegexTimeStamp + "Log MyFile\\:123 \\(MyFunction\\)\\: Fake error\\: Error code\\: 2 \\(02\\)\\: No such file or directory"));
}

TEST_F(TracingTest, ThrowGenericError)
{
    utility::GenericError error("Fake error");

    try
    {
        Tracing::Throw("MyFile", 123, "MyFunction", error);
    }
    catch (std::exception & e)
    {
        m_traceOutput = e.what();        
    }
    EXPECT_TRUE(utility::VerifyMatch(m_traceOutput, 
        TraceRegexTimeStamp + "Log MyFile\\:123 \\(MyFunction\\)\\: Fake error"));
}

TEST_F(TracingTest, LogErrorString)
{
    SetDefaultTraceFilter(TraceCategory::All);
    LogError("MyFile", 123, "MyFunction", "Hello World");
    EXPECT_TRUE(utility::VerifyMatch(m_traceOutput, 
        TraceRegexTimeStamp + "Error\\|MyFile\\:123\\|MyFunction\\|TracingTest\\|Hello World\n"));
}

TEST_F(TracingTest, LogErrorFormatted)
{
    SetDefaultTraceFilter(TraceCategory::All);
    LogError("MyFile", 123, "MyFunction", "{0} {1}", "Hello", "World");
    EXPECT_TRUE(utility::VerifyMatch(m_traceOutput, 
        TraceRegexTimeStamp + "Error\\|MyFile\\:123\\|MyFunction\\|TracingTest\\|Hello World\n"));
}

TEST_F(TracingTest, LogFatalString)
{
    SetDefaultTraceFilter(TraceCategory::All);
    LogFatal("MyFile", 123, "MyFunction", "Hello World");
    EXPECT_TRUE(utility::VerifyMatch(m_traceOutput, 
        TraceRegexTimeStamp + "Fatal\\|MyFile\\:123\\|MyFunction\\|TracingTest\\|Hello World\n"));
}

TEST_F(TracingTest, LogFatalFormatted)
{
    SetDefaultTraceFilter(TraceCategory::All);
    LogFatal("MyFile", 123, "MyFunction", "{0} {1}", "Hello", "World");
    EXPECT_TRUE(utility::VerifyMatch(m_traceOutput, 
        TraceRegexTimeStamp + "Fatal\\|MyFile\\:123\\|MyFunction\\|TracingTest\\|Hello World\n"));
}

} // namespace tracing
