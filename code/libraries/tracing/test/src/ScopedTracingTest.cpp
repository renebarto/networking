#include "GoogleTest.h"

#include "tracing/ScopedTracing.h"
#include "tracing/TraceCategory.h"
#include "core/Regex.h"

namespace tracing {

static const std::string TraceRegexTimeStamp = "\\d{4}-\\w{3}-\\d{2}\\s\\d{2}:\\d{2}:\\d{2}\\.\\d{6}\\|";

class ScopedTracingTest : public ::testing::Test
{
public:
    std::string m_traceOutput;
    CategorySet<TraceCategory> m_savedTraceFilter;

    ScopedTracingTest()
        : m_traceOutput()
        , m_savedTraceFilter()
    {}

    virtual void SetUp() override
    {
        tracing::Tracing::SetTracingFunction(
            std::bind(&ScopedTracingTest::TraceFunc, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, 
                      std::placeholders::_4, std::placeholders::_5, std::placeholders::_6));
        m_savedTraceFilter = GetDefaultTraceFilter();
    }
    virtual void TearDown() override
    {
        tracing::Tracing::SetTracingFunction(nullptr);
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

TEST_F(ScopedTracingTest, IfNothingEnabledNothingHappens)
{
    SetDefaultTraceFilter(TraceCategory::None);
    {
        ScopedTracing trace("MyFile", 123, "MyFunction", [](){ return "This is the begin"; }, [](){ return "This is the end"; });
    }
    EXPECT_EQ("", m_traceOutput);
}

TEST_F(ScopedTracingTest, IfEnabledEntryAndExitMessageWritten)
{
    SetDefaultTraceFilter(TraceCategory::All);
    {
        ScopedTracing trace("MyFile", 123, "MyFunction", [](){ return "This is the begin"; }, [](){ return "This is the end"; });
    }
    EXPECT_TRUE(core::VerifyMatch(m_traceOutput, 
        TraceRegexTimeStamp + "Enter\\|MyFile\\:123\\|MyFunction\\|This is the begin\n" +
        TraceRegexTimeStamp + "Leave\\|MyFile\\:123\\|MyFunction\\|This is the end\n"));
}

TEST_F(ScopedTracingTest, IfExitFunctionIsNullEmptyStringIsWritten)
{
    SetDefaultTraceFilter(TraceCategory::All);
    {
        ScopedTracing trace("MyFile", 123, "MyFunction", nullptr, nullptr);
    }
    EXPECT_TRUE(core::VerifyMatch(m_traceOutput, 
        TraceRegexTimeStamp + "Enter\\|MyFile\\:123\\|MyFunction\\|\n" +
        TraceRegexTimeStamp + "Leave\\|MyFile\\:123\\|MyFunction\\|\n"));
}

} // namespace tracing
