#include "GoogleTest.h"

#include "core/Regex.h"
#include "tracing/Tracing.h"

namespace tracing {

static const std::string TraceRegexTimeStamp = "\\d{4}-\\w{3}-\\d{2}\\s\\d{2}:\\d{2}:\\d{2}\\.\\d{6}\\|";

class TracingTest : public ::testing::Test
{
public:
    std::string m_traceOutput;
    CategorySet<TraceCategory> m_savedTraceFilter;

    TracingTest()
        : m_traceOutput()
        , m_savedTraceFilter()
    {}

    virtual void SetUp() override
    {
        Tracing::SetTracingFunction(
            std::bind(&TracingTest::TraceFunc, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, 
                      std::placeholders::_4, std::placeholders::_5, std::placeholders::_6));
        m_savedTraceFilter = GetDefaultTraceFilter();
    }
    virtual void TearDown() override
    {
        Tracing::SetTracingFunction(nullptr);
        SetDefaultTraceFilter(m_savedTraceFilter);
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
        stream << osal::Clock::ToString(timestamp) << "|" << category << "|" << fileName << ":" << line << "|" << functionName << "|" << msg << std::endl;
        m_traceOutput += stream.str();
    }
};

TEST_F(TracingTest, IfTracingFunctionsSetButNothingEnabledNothingHappens)
{
    SetDefaultTraceFilter(TraceCategory::None);
    Tracing::Trace(TraceCategory::Startup, "MyFile", 123, "MyFunction", "Hello World");
    EXPECT_EQ("", m_traceOutput);
}

TEST_F(TracingTest, IfTracingFunctionsSetAndCategoryEnabledTraceIsWritten)
{
    SetDefaultTraceFilter(TraceCategory::All);
    Tracing::Trace(TraceCategory::Startup, "MyFile", 123, "MyFunction", "Hello World");
    EXPECT_TRUE(core::VerifyMatch(m_traceOutput, 
        TraceRegexTimeStamp + "Start\\|MyFile\\:123\\|MyFunction\\|Hello World\n"));
}

TEST_F(TracingTest, TracingOnlyForEnabledCategory)
{
    SetDefaultTraceFilter(TraceCategory::Startup);
    Tracing::Trace(TraceCategory::Startup, "MyFile", 123, "MyFunction", "Hello World");
    Tracing::Trace(TraceCategory::Shutdown, "MyFile", 123, "MyFunction", "Hello World");
    EXPECT_TRUE(core::VerifyMatch(m_traceOutput, 
        TraceRegexTimeStamp + "Start\\|MyFile\\:123\\|MyFunction\\|Hello World\n"));
}

TEST_F(TracingTest, IfNoTracingEnableFunctionSetOnlyDefaultCategoriesAreEnabled)
{
    // Assumption: default only Startup and Shutdown enabled
    Tracing::Trace(TraceCategory::Startup, "MyFile", 123, "MyFunction", "Hello World");
    Tracing::Trace(TraceCategory::Shutdown, "MyFile", 123, "MyFunction", "Hello World");
    Tracing::Trace(TraceCategory::Data, "MyFile", 123, "MyFunction", "Hello World");
    EXPECT_TRUE(core::VerifyMatch(m_traceOutput, 
        TraceRegexTimeStamp + "Start\\|MyFile\\:123\\|MyFunction\\|Hello World\n" +
        TraceRegexTimeStamp + "Shtdn\\|MyFile\\:123\\|MyFunction\\|Hello World\n"));
}

TEST_F(TracingTest, IfDefaultFilterIsChangedOnlySpecifiedCategoriesAreEnabled)
{
    SetDefaultTraceFilter(TraceCategory::Startup);
    Tracing::Trace(TraceCategory::Startup, "MyFile", 123, "MyFunction", "Hello World");
    Tracing::Trace(TraceCategory::Shutdown, "MyFile", 123, "MyFunction", "Hello World");
    Tracing::Trace(TraceCategory::Data, "MyFile", 123, "MyFunction", "Hello World");
    EXPECT_TRUE(core::VerifyMatch(m_traceOutput, 
        TraceRegexTimeStamp + "Start\\|MyFile\\:123\\|MyFunction\\|Hello World\n"));
}

TEST_F(TracingTest, TracingCategories)
{
    SetDefaultTraceFilter(TraceCategory::All);
    Tracing::Trace(TraceCategory::FunctionEnter, "MyFile", 123, "MyFunction", "Hello World");
    Tracing::Trace(TraceCategory::FunctionLeave, "MyFile", 123, "MyFunction", "Hello World");
    Tracing::Trace(TraceCategory::Startup, "MyFile", 123, "MyFunction", "Hello World");
    Tracing::Trace(TraceCategory::Shutdown, "MyFile", 123, "MyFunction", "Hello World");
    Tracing::Trace(TraceCategory::Log, "MyFile", 123, "MyFunction", "Hello World");
    Tracing::Trace(TraceCategory::Message, "MyFile", 123, "MyFunction", "Hello World");
    Tracing::Trace(TraceCategory::Data, "MyFile", 123, "MyFunction", "Hello World");
    EXPECT_TRUE(core::VerifyMatch(m_traceOutput, 
        TraceRegexTimeStamp + "Enter\\|MyFile\\:123\\|MyFunction\\|Hello World\n" + 
        TraceRegexTimeStamp + "Leave\\|MyFile\\:123\\|MyFunction\\|Hello World\n" + 
        TraceRegexTimeStamp + "Start\\|MyFile\\:123\\|MyFunction\\|Hello World\n" + 
        TraceRegexTimeStamp + "Shtdn\\|MyFile\\:123\\|MyFunction\\|Hello World\n" + 
        TraceRegexTimeStamp + "Log  \\|MyFile\\:123\\|MyFunction\\|Hello World\n" + 
        TraceRegexTimeStamp + "Messg\\|MyFile\\:123\\|MyFunction\\|Hello World\n" + 
        TraceRegexTimeStamp + "Data \\|MyFile\\:123\\|MyFunction\\|Hello World\n"));
}

TEST_F(TracingTest, TracingWithFormat)
{
    SetDefaultTraceFilter(TraceCategory::Message);
    Tracing::Trace(TraceCategory::Message, "MyFile", 123, "MyFunction", "{0} {1} (C) {2,4:D}", "Hello", "World", 2020);
    EXPECT_TRUE(core::VerifyMatch(m_traceOutput, 
        TraceRegexTimeStamp + "Messg\\|MyFile\\:123\\|MyFunction\\|Hello World \\(C\\) 2020\n"));
}

TEST_F(TracingTest, TraceFuncEnterString)
{
    SetDefaultTraceFilter(TraceCategory::All);
    TraceFuncEnter("MyFile", 123, "MyFunction", "Hello World");
    EXPECT_TRUE(core::VerifyMatch(m_traceOutput, 
        TraceRegexTimeStamp + "Enter\\|MyFile\\:123\\|MyFunction\\|Hello World\n"));
}

TEST_F(TracingTest, TraceFuncEnterFormatted)
{
    SetDefaultTraceFilter(TraceCategory::All);
    TraceFuncEnter("MyFile", 123, "MyFunction", "{0} {1}", "Hello", "World");
    EXPECT_TRUE(core::VerifyMatch(m_traceOutput, 
        TraceRegexTimeStamp + "Enter\\|MyFile\\:123\\|MyFunction\\|Hello World\n"));
}

TEST_F(TracingTest, TraceFuncLeaveString)
{
    SetDefaultTraceFilter(TraceCategory::All);
    TraceFuncLeave("MyFile", 123, "MyFunction", "Hello World");
    EXPECT_TRUE(core::VerifyMatch(m_traceOutput, 
        TraceRegexTimeStamp + "Leave\\|MyFile\\:123\\|MyFunction\\|Hello World\n"));
}

TEST_F(TracingTest, TraceFuncLeaveFormatted)
{
    SetDefaultTraceFilter(TraceCategory::All);
    TraceFuncLeave("MyFile", 123, "MyFunction", "{0} {1}", "Hello", "World");
    EXPECT_TRUE(core::VerifyMatch(m_traceOutput, 
        TraceRegexTimeStamp + "Leave\\|MyFile\\:123\\|MyFunction\\|Hello World\n"));
}

TEST_F(TracingTest, TraceStartup)
{
    SetDefaultTraceFilter(TraceCategory::All);
    TraceStartup("MyFile", 123, "MyFunction", "Hello World");
    EXPECT_TRUE(core::VerifyMatch(m_traceOutput, 
        TraceRegexTimeStamp + "Start\\|MyFile\\:123\\|MyFunction\\|Hello World\n"));
}

TEST_F(TracingTest, TraceShutdown)
{
    SetDefaultTraceFilter(TraceCategory::All);
    TraceShutdown("MyFile", 123, "MyFunction", "Hello World");
    EXPECT_TRUE(core::VerifyMatch(m_traceOutput, 
        TraceRegexTimeStamp + "Shtdn\\|MyFile\\:123\\|MyFunction\\|Hello World\n"));
}

TEST_F(TracingTest, TraceMessageString)
{
    SetDefaultTraceFilter(TraceCategory::All);
    TraceMessage("MyFile", 123, "MyFunction", "Hello World");
    EXPECT_TRUE(core::VerifyMatch(m_traceOutput, 
        TraceRegexTimeStamp + "Messg\\|MyFile\\:123\\|MyFunction\\|Hello World\n"));
}

TEST_F(TracingTest, TraceMessageFormatted)
{
    SetDefaultTraceFilter(TraceCategory::All);
    TraceMessage("MyFile", 123, "MyFunction", "{0} {1}", "Hello", "World");
    EXPECT_TRUE(core::VerifyMatch(m_traceOutput, 
        TraceRegexTimeStamp + "Messg\\|MyFile\\:123\\|MyFunction\\|Hello World\n"));
}

TEST_F(TracingTest, TraceDataString)
{
    SetDefaultTraceFilter(TraceCategory::All);
    TraceData("MyFile", 123, "MyFunction", "Hello World");
    EXPECT_TRUE(core::VerifyMatch(m_traceOutput, 
        TraceRegexTimeStamp + "Data \\|MyFile\\:123\\|MyFunction\\|Hello World\n"));
}

TEST_F(TracingTest, TraceDataFormatted)
{
    SetDefaultTraceFilter(TraceCategory::All);
    TraceData("MyFile", 123, "MyFunction", "{0} {1}", "Hello", "World");
    EXPECT_TRUE(core::VerifyMatch(m_traceOutput, 
        TraceRegexTimeStamp + "Data \\|MyFile\\:123\\|MyFunction\\|Hello World\n"));
}

TEST_F(TracingTest, TraceToConsole)
{
    SetDefaultTraceFilter(TraceCategory::All);
    TraceStartup("MyFile", 123, "MyFunction", "Starting");
    TraceShutdown("MyFile", 123, "MyFunction", "Stopping");
}

} // namespace tracing
