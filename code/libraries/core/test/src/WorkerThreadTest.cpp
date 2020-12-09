#include <GoogleTest.h>

// #include "osal/Time.h"
#include "core/WorkerThread.h"

namespace core {

static auto SLEEP = std::chrono::milliseconds(200);
static auto TIMEOUT = std::chrono::milliseconds(50);

class MyWorkerThread: public WorkerThread
{
public:
    static bool m_isDeleted;
    bool m_isFlagged;

    MyWorkerThread()
        : WorkerThread("MyWorkerThread")
        , m_isFlagged()
    {
        m_isDeleted = false;
    }
    ~MyWorkerThread()
    {
        m_isDeleted = true;
    }
    void Thread() override
    {
        m_isFlagged = true;
        std::this_thread::sleep_for(SLEEP);
    }
};

bool MyWorkerThread::m_isDeleted;

class WorkerThreadTest : public ::testing::Test
{
public:
    virtual void SetUp() {}
    virtual void TearDown() {}
};

TEST_F(WorkerThreadTest, Construction)
{
    MyWorkerThread thread;
    EXPECT_FALSE(thread.IsRunning());
    EXPECT_FALSE(thread.WaitForBirth(TIMEOUT));
    std::this_thread::sleep_for(SLEEP);
    EXPECT_FALSE(thread.m_isFlagged);
    std::this_thread::sleep_for(SLEEP);
    EXPECT_FALSE(MyWorkerThread::m_isDeleted);
}

TEST_F(WorkerThreadTest, Create)
{
    MyWorkerThread thread;

    thread.Create();
    EXPECT_TRUE(thread.IsRunning());
    EXPECT_TRUE(thread.WaitForBirth(TIMEOUT));
    std::this_thread::sleep_for(SLEEP);
    EXPECT_TRUE(thread.m_isFlagged);
    std::this_thread::sleep_for(SLEEP);
    EXPECT_FALSE(MyWorkerThread::m_isDeleted);
}

} // namespace core
