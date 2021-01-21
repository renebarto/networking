#include "GoogleTest.h"

#include "osal/Thread.h"
#include "core/Thread.h"
#include "tracing/Tracing.h"

namespace core {
namespace thread {

static const auto SLEEP = std::chrono::milliseconds(50);
static const auto TIMEOUT = std::chrono::milliseconds(50);
static const std::string ThreadName = "MyThread";

class ThreadTest : public ::testing::Test
{
public:
    tracing::CategorySet<tracing::TraceCategory> m_savedTraceFilter;

    ThreadTest()
        : m_savedTraceFilter()
    {}

    virtual void SetUp() override
    {
        tracing::Tracing::SetTracingFunction(
            std::bind(&ThreadTest::TraceFunc, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, 
                      std::placeholders::_4, std::placeholders::_5, std::placeholders::_6));
        m_savedTraceFilter = tracing::GetDefaultTraceFilter();
    }
    virtual void TearDown() override 
    {
        tracing::SetDefaultTraceFilter(m_savedTraceFilter);
    }

    void TraceFunc(
        osal::EpochTime timestamp,
        tracing::TraceCategory category,
        const std::string & fileName, 
        int line, 
        const std::string & functionName, 
        const std::string & msg)
    {
        std::cout << osal::Clock::ToString(timestamp) << "|" << category << "|" << fileName << ":" << line << "|" << functionName << "|" << osal::GetThreadNameSelf() << "|" << msg << std::endl;
    }
    static void TestThread()
    {
        std::this_thread::sleep_for(SLEEP);
    }
    static void CrashingThread()
    {
        std::this_thread::sleep_for(SLEEP);
        throw std::runtime_error("Crash");
    }
};

TEST_F(ThreadTest, Construction)
{
    Thread thread;
    EXPECT_FALSE(thread.IsAlive());
    EXPECT_FALSE(thread.IsRunning());
    EXPECT_FALSE(thread.IsFinished());
    EXPECT_EQ("", thread.GetName());
    thread.WaitForDeath();
    EXPECT_FALSE(thread.IsAlive());
    EXPECT_FALSE(thread.IsRunning());
    EXPECT_FALSE(thread.IsFinished());
    EXPECT_FALSE(thread.HasDied());
    EXPECT_FALSE(thread.HaveResult());
    EXPECT_THROW(thread.GetResult(), std::future_error);
}

TEST_F(ThreadTest, ConstructionName)
{
    Thread thread(ThreadName);
    EXPECT_FALSE(thread.IsAlive());
    EXPECT_FALSE(thread.IsRunning());
    EXPECT_FALSE(thread.IsFinished());
    EXPECT_EQ(ThreadName, thread.GetName());
    thread.WaitForDeath();
    EXPECT_FALSE(thread.IsAlive());
    EXPECT_FALSE(thread.IsRunning());
    EXPECT_FALSE(thread.IsFinished());
    EXPECT_FALSE(thread.HasDied());
    EXPECT_FALSE(thread.HaveResult());
    EXPECT_THROW(thread.GetResult(), std::future_error);
}

TEST_F(ThreadTest, ConstructionFunction)
{
    Thread thread(TestThread);
    EXPECT_TRUE(thread.IsAlive());
    EXPECT_TRUE(thread.IsRunning());
    EXPECT_FALSE(thread.IsFinished());
    EXPECT_EQ("", thread.GetName());
    std::this_thread::sleep_for(SLEEP);
    thread.WaitForDeath();
    EXPECT_FALSE(thread.IsAlive());
    EXPECT_FALSE(thread.IsRunning());
    EXPECT_TRUE(thread.IsFinished());
    EXPECT_TRUE(thread.HasDied());
    EXPECT_EQ("", thread.GetName());
    EXPECT_TRUE(thread.HaveResult());
    EXPECT_NO_THROW(thread.GetResult());
}

TEST_F(ThreadTest, ConstructionFunctionName)
{
    Thread thread(TestThread, ThreadName);
    EXPECT_TRUE(thread.IsAlive());
    EXPECT_TRUE(thread.IsRunning());
    EXPECT_FALSE(thread.IsFinished());
    EXPECT_EQ(ThreadName, thread.GetName());
    std::this_thread::sleep_for(SLEEP);
    thread.WaitForDeath();
    EXPECT_FALSE(thread.IsAlive());
    EXPECT_FALSE(thread.IsRunning());
    EXPECT_TRUE(thread.IsFinished());
    EXPECT_TRUE(thread.HasDied());
    EXPECT_EQ(ThreadName, thread.GetName());
    EXPECT_TRUE(thread.HaveResult());
    EXPECT_NO_THROW(thread.GetResult());
}

TEST_F(ThreadTest, GetSetName)
{
    const std::string AltName = "Thread";
    Thread thread(ThreadName);
    EXPECT_EQ(ThreadName, thread.GetName());
    thread.Create(TestThread);
    thread.SetName(AltName);
    EXPECT_EQ(AltName, thread.GetName());
    EXPECT_TRUE(thread.HaveResult());
    EXPECT_NO_THROW(thread.GetResult());
}

TEST_F(ThreadTest, Create)
{
    Thread thread(ThreadName);
    EXPECT_FALSE(thread.IsAlive());
    EXPECT_FALSE(thread.IsRunning());
    EXPECT_FALSE(thread.IsFinished());
    EXPECT_EQ(ThreadName, thread.GetName());
    thread.Create(TestThread);
    EXPECT_TRUE(thread.IsAlive());
    EXPECT_TRUE(thread.IsRunning());
    EXPECT_FALSE(thread.IsFinished());
    EXPECT_EQ(ThreadName, thread.GetName());
    std::this_thread::sleep_for(SLEEP);
    thread.WaitForDeath();
    EXPECT_FALSE(thread.IsAlive());
    EXPECT_FALSE(thread.IsRunning());
    EXPECT_TRUE(thread.IsFinished());
    EXPECT_TRUE(thread.HasDied());
    EXPECT_EQ(ThreadName, thread.GetName());
    EXPECT_TRUE(thread.HaveResult());
    EXPECT_NO_THROW(thread.GetResult());
}

TEST_F(ThreadTest, Destroy)
{
    Thread thread(TestThread, ThreadName);
    EXPECT_TRUE(thread.IsAlive());
    EXPECT_TRUE(thread.IsRunning());
    EXPECT_FALSE(thread.IsFinished());
    EXPECT_EQ(ThreadName, thread.GetName());
    std::this_thread::sleep_for(SLEEP);
    thread.Destroy();
    EXPECT_FALSE(thread.IsAlive());
    EXPECT_FALSE(thread.IsRunning());
    EXPECT_FALSE(thread.IsFinished());
    EXPECT_TRUE(thread.HasDied());
    EXPECT_EQ(ThreadName, thread.GetName());
    EXPECT_TRUE(thread.HaveResult());
    EXPECT_NO_THROW(thread.GetResult());
}

TEST_F(ThreadTest, CrashingThread)
{
    Thread thread(CrashingThread, ThreadName);
    EXPECT_TRUE(thread.IsAlive());
    EXPECT_TRUE(thread.IsRunning());
    EXPECT_FALSE(thread.IsFinished());
    EXPECT_EQ(ThreadName, thread.GetName());
    std::this_thread::sleep_for(SLEEP);
    thread.Destroy();
    EXPECT_FALSE(thread.IsAlive());
    EXPECT_FALSE(thread.IsRunning());
    EXPECT_FALSE(thread.IsFinished());
    EXPECT_TRUE(thread.HasDied());
    EXPECT_EQ(ThreadName, thread.GetName());
    EXPECT_TRUE(thread.HaveResult());
    EXPECT_THROW(thread.GetResult(), std::runtime_error);
}

} // namespace thread
} // namespace core
