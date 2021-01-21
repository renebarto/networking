#include "core/Thread.h"

#include "tracing/Logging.h"
#include "tracing/ScopedTracing.h"

namespace core {

Thread::Thread()
    : m_threadMutex()
    , m_thread()
    , m_name()
    , m_state(ThreadState::NotStarted)
{
    SCOPEDTRACE(nullptr, nullptr);
}

Thread::Thread(const std::string & name)
    : m_threadMutex()
    , m_thread()
    , m_name(name.substr(0, std::min(name.length(), size_t {15})))
    , m_state(ThreadState::NotStarted)
{
    SCOPEDTRACE([&] () { return utility::FormatString(std::string("name={0}"), m_name); }, nullptr);
}

Thread::Thread(ThreadFunction threadFunc)
    : m_threadMutex()
    , m_thread()
    , m_name()
    , m_state(ThreadState::NotStarted)
{
    SCOPEDTRACE(nullptr, nullptr);
    Create(threadFunc);
}

Thread::Thread(ThreadFunction threadFunc, const std::string & name)
    : m_threadMutex()
    , m_thread()
    , m_name(name.substr(0, std::min(name.length(), size_t {15})))
    , m_state(ThreadState::NotStarted)
{
    SCOPEDTRACE([&] () { return utility::FormatString(std::string("name={0}"), m_name); }, nullptr);
    Create(threadFunc);
}

Thread::~Thread()
{
    SCOPEDTRACE(nullptr, nullptr);
    Cleanup();
}

void Thread::Create(ThreadFunction threadFunc)
{
    SCOPEDTRACE(nullptr, nullptr);

    Destroy();

    Lock lock(m_threadMutex);
    try
    {
        std::packaged_task<void()> task(std::bind(threadFunc));

        m_threadResult = task.get_future();
        m_thread = std::thread(std::move(task));
        m_state = ThreadState::Running;
        osal::SetThreadName(m_thread, m_name);
        TraceMessage(__FILE__, __LINE__, __func__, "Start thread {}, id {}", m_name, m_thread.get_id());
    }
    catch (const std::exception & e)
    {
        tracing::Logging::Log(tracing::LogCategory::Error, __FILE__, __LINE__, __func__, e.what());
        Cleanup();
        throw;
    }
}

void Thread::Destroy()
{
    SCOPEDTRACE(nullptr, nullptr);

    Lock lock(m_threadMutex);
    if (IsAlive())
    {
        WaitForDeath();
        m_state = ThreadState::Killed;
    }
}

bool Thread::IsAlive()
{
    bool result;
    SCOPEDTRACE(nullptr, [&](){ return utility::FormatString(std::string("result={}"), result); });

    Lock lock(m_threadMutex);
 
    result = osal::IsThreadAlive(m_thread);

    return result;
}

bool Thread::IsRunning()
{
    bool result;
    SCOPEDTRACE(nullptr, [&](){ return utility::FormatString(std::string("result={}"), result); });

    Lock lock(m_threadMutex);

    result = osal::IsThreadAlive(m_thread) && (m_state == ThreadState::Running);

    return result;
}

bool Thread::IsFinished()
{
    bool result;
    SCOPEDTRACE(nullptr, [&](){ return utility::FormatString(std::string("result={}"), result); });

    Lock lock(m_threadMutex);

    result = (m_state == ThreadState::Finished);

    return result;
}

bool Thread::HasDied()
{
    bool result;
    SCOPEDTRACE(nullptr, [&](){ return utility::FormatString(std::string("result={}"), result); });

    Lock lock(m_threadMutex);

    result = (m_state == ThreadState::Finished) || (m_state == ThreadState::Killed);

    return result;
}

const std::string & Thread::GetName() const
{
    SCOPEDTRACE(nullptr, [&](){ return utility::FormatString(std::string("result={}"), m_name); });

    return m_name;
}

void Thread::SetName(const std::string & name)
{
    SCOPEDTRACE([&] () { return utility::FormatString(std::string("name={}"), name); }, nullptr);
    m_name = name.substr(0, std::min(name.length(), size_t {15}));
    osal::SetThreadName(m_thread, m_name);
}

void Thread::WaitForDeath()
{
    SCOPEDTRACE(nullptr, nullptr);

    if (!IsAlive())
        return;
    Lock lock(m_threadMutex);
    if (!osal::IsThreadSelf(m_thread))
    {
        TraceMessage(__FILE__, __LINE__, __func__, "Wait for thread to end");
        m_thread.join();
        TraceMessage(__FILE__, __LINE__, __func__, "Thread ended");
        m_state = ThreadState::Finished;
    }
}

void Thread::Cleanup()
{
    SCOPEDTRACE(nullptr, nullptr);

    Destroy();
    Lock lock(m_threadMutex);
    m_state = ThreadState::NotStarted;
}

osal::ThreadPriority Thread::GetPriority()
{
    osal::ThreadPriority priority {};
    SCOPEDTRACE(nullptr, [&]() { return utility::FormatString(std::string("result={}"), static_cast<int>(priority)); });

    priority = osal::GetThreadPriority(m_thread);
    return priority;
}

void Thread::SetPriority(osal::ThreadPriority priority)
{
    SCOPEDTRACE([&] () { return utility::FormatString(std::string("priority={}"), static_cast<int>(priority)); }, nullptr);
    osal::SetThreadPriority(m_thread, priority);
}

} // namespace core