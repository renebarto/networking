#include "GoogleTest.h"

#include <iostream>
#include <thread>
#include "core/ActiveObject.h"

namespace core {

static auto SLEEP = std::chrono::milliseconds(100);

class MyActiveObject
    : public ActiveObject
{
public:
    static bool m_isDeleted;

    MyActiveObject()
        : ActiveObject("ActiveObjTst1")
    {
        m_isDeleted = false;
    }
    ~MyActiveObject()
    {
        m_isDeleted = true;
    }
    virtual void InitThread() {}
    virtual void Run() { std::this_thread::sleep_for(SLEEP); }
    virtual void ExitThread() {}
    virtual void FlushThread() {}
};

bool MyActiveObject::m_isDeleted;

class MyActiveObject2
    : public ActiveObject
{
public:
    static bool m_isDeleted;

    MyActiveObject2()
        : ActiveObject("ActiveObjTst2")
    {
        m_isDeleted = false;
    }
    ~MyActiveObject2()
    {
        m_isDeleted = true;
    }

    virtual void InitThread() {}

    virtual void Run()
    {
        while (!IsDying())
        {
            std::this_thread::sleep_for(SLEEP * 2);
        }
    }
    virtual void ExitThread() {}
    virtual void FlushThread() {}
};

bool MyActiveObject2::m_isDeleted;

class ActiveObjectTest
    : public ::testing::Test
{
public:
    tracing::CategorySet<tracing::TraceCategory> m_savedTraceFilter;

    ActiveObjectTest()
        : m_savedTraceFilter()
    {}

    void SetUp() override
    {
        m_savedTraceFilter = tracing::GetDefaultTraceFilter();
        tracing::SetDefaultTraceFilter(tracing::TraceCategory::Message);
    }
    void TearDown() override
    {
        tracing::SetDefaultTraceFilter(m_savedTraceFilter);
    }
};

TEST_F(ActiveObjectTest, Simple)
{
    {
        MyActiveObject object;
        object.Create();
        std::this_thread::sleep_for(SLEEP);
        object.Kill();
    }
    EXPECT_TRUE(MyActiveObject::m_isDeleted);
}

TEST_F(ActiveObjectTest, Loop)
{
    {
        MyActiveObject2 object;
        object.Create();
        std::this_thread::sleep_for(SLEEP);
        object.Kill();
    }
    EXPECT_TRUE(MyActiveObject::m_isDeleted);
}

} // namespace core
