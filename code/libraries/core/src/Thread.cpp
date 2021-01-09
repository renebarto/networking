#include "core/Thread.h"

#include "osal/Thread.h"

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
        SCOPEDTRACE([] () { return "Start thread"; }, nullptr);
        std::packaged_task<void()> task(std::bind(threadFunc));

        m_threadResult = task.get_future();
        m_thread = std::thread(std::move(task));
        m_state = ThreadState::Running;
        SetThreadName(m_thread, m_name);

    }
    catch (const std::exception & e)
    {
        tracing::Tracing::Trace(tracing::TraceCategory::Error, __FILE__, __LINE__, __func__, e.what());
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
 
    result = IsThreadAlive(m_thread);

    return result;
}

bool Thread::IsRunning()
{
    bool result;
    SCOPEDTRACE(nullptr, [&](){ return utility::FormatString(std::string("result={}"), result); });

    Lock lock(m_threadMutex);

    result = IsThreadAlive(m_thread) && (m_state == ThreadState::Running);

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
    SetThreadName(m_thread, m_name);
}

void Thread::WaitForDeath()
{
    SCOPEDTRACE(nullptr, nullptr);

    if (!IsAlive())
        return;
    Lock lock(m_threadMutex);
    if (!IsThreadSelf(m_thread))
    {
        SCOPEDTRACE([] () { return "Wait for thread to die"; }, nullptr);
        m_thread.join();
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

Priority Thread::GetPriority()
{
    Priority priority {};
    SCOPEDTRACE(nullptr, [&]() { return utility::FormatString(std::string("result={}"), static_cast<int>(priority)); });

    priority = GetPriority(m_thread);
    return priority;
}

void Thread::SetPriority(Priority priority)
{
    SCOPEDTRACE([&] () { return utility::FormatString(std::string("priority={}"), static_cast<int>(priority)); }, nullptr);
    SetPriority(m_thread, priority);
}

#if defined(PLATFORM_LINUX)  || defined(PLATFORM_LINUX_RPI)

Priority Thread::GetPriorityCurrentThread()
{
    int policy;
    struct sched_param sched;
    int result = pthread_getschedparam(pthread_self(), &policy, &sched);
    if (result != 0)
        throw std::runtime_error("Failed to get the current thread priority!");
    if ((policy == SCHED_FIFO) || (policy == SCHED_RR))
    {
        if (sched.sched_priority < 15)
            return Priority::IDLE;
        else if (sched.sched_priority < 30)
            return Priority::LOWEST;
        else if (sched.sched_priority < 50)
            return Priority::LOW;
        else if (sched.sched_priority < 70)
            return Priority::NORMAL;
        else if (sched.sched_priority < 85)
            return Priority::HIGH;
        else if (sched.sched_priority < 99)
            return Priority::HIGHEST;
        else
            return Priority::REALTIME;
    } else
        return Priority::NORMAL;
}

Priority Thread::GetPriority(std::thread & thread)
{
    int policy;
    struct sched_param sched;
    int result = pthread_getschedparam(thread.native_handle(), &policy, &sched);
    if (result != 0)
        throw std::runtime_error("Failed to get the given thread priority!");
    if ((policy == SCHED_FIFO) || (policy == SCHED_RR))
    {
        if (sched.sched_priority < 15)
            return Priority::IDLE;
        else if (sched.sched_priority < 30)
            return Priority::LOWEST;
        else if (sched.sched_priority < 50)
            return Priority::LOW;
        else if (sched.sched_priority < 70)
            return Priority::NORMAL;
        else if (sched.sched_priority < 85)
            return Priority::HIGH;
        else if (sched.sched_priority < 99)
            return Priority::HIGHEST;
        else
            return Priority::REALTIME;
    } else
        return Priority::NORMAL;
}

void Thread::SetPriorityCurrentThread(Priority priority)
{
    int policy = SCHED_RR;
    struct sched_param sched;
    sched.sched_priority = 50;
    switch (priority)
    {
        case Priority::IDLE:
            sched.sched_priority = 1;
            break;
        case Priority::LOWEST:
            sched.sched_priority = 15;
            break;
        case Priority::LOW:
            sched.sched_priority = 30;
            break;
        case Priority::NORMAL:
            sched.sched_priority = 50;
            break;
        case Priority::HIGH:
            sched.sched_priority = 70;
            break;
        case Priority::HIGHEST:
            sched.sched_priority = 85;
            break;
        case Priority::REALTIME:
            sched.sched_priority = 99;
            break;
    }

    int result = pthread_setschedparam(pthread_self(), policy, &sched);
    if (result != 0)
        throw std::runtime_error("Failed to set the current thread priority!");
}

void Thread::SetPriority(std::thread & thread, Priority priority)
{
    int policy = SCHED_RR;
    struct sched_param sched;
    sched.sched_priority = 50;
    switch (priority)
    {
        case Priority::IDLE:
            sched.sched_priority = 1;
            break;
        case Priority::LOWEST:
            sched.sched_priority = 15;
            break;
        case Priority::LOW:
            sched.sched_priority = 30;
            break;
        case Priority::NORMAL:
            sched.sched_priority = 50;
            break;
        case Priority::HIGH:
            sched.sched_priority = 70;
            break;
        case Priority::HIGHEST:
            sched.sched_priority = 85;
            break;
        case Priority::REALTIME:
            sched.sched_priority = 99;
            break;
    }

    int result = pthread_setschedparam(thread.native_handle(), policy, &sched);
    if (result != 0)
        throw std::runtime_error("Failed to set the given thread priority!");
}

#elif defined(PLATFORM_WINDOWS)

Priority Thread::GetPriorityCurrentThread()
{
    int priority = GetThreadPriority(GetCurrentThread());
    if (priority == THREAD_PRIORITY_ERROR_RETURN)
        throw std::runtime_error("Failed to get the current thread priority!");
    if (priority < THREAD_PRIORITY_LOWEST)
        return Priority::IDLE;
    else if (priority < THREAD_PRIORITY_BELOW_NORMAL)
        return Priority::LOWEST;
    else if (priority < THREAD_PRIORITY_NORMAL)
        return Priority::LOW;
    else if (priority < THREAD_PRIORITY_ABOVE_NORMAL)
        return Priority::NORMAL;
    else if (priority < THREAD_PRIORITY_HIGHEST)
        return Priority::HIGH;
    else if (priority < THREAD_PRIORITY_TIME_CRITICAL)
        return Priority::HIGHEST;
    else
        return Priority::REALTIME;
}

Priority Thread::GetPriority(std::thread & thread)
{
    int priority = GetThreadPriority(static_cast<HANDLE>(thread.native_handle()));
    if (priority == THREAD_PRIORITY_ERROR_RETURN)
        throw std::runtime_error("Failed to get the given thread priority!");
    if (priority < THREAD_PRIORITY_LOWEST)
        return Priority::IDLE;
    else if (priority < THREAD_PRIORITY_BELOW_NORMAL)
        return Priority::LOWEST;
    else if (priority < THREAD_PRIORITY_NORMAL)
        return Priority::LOW;
    else if (priority < THREAD_PRIORITY_ABOVE_NORMAL)
        return Priority::NORMAL;
    else if (priority < THREAD_PRIORITY_HIGHEST)
        return Priority::HIGH;
    else if (priority < THREAD_PRIORITY_TIME_CRITICAL)
        return Priority::HIGHEST;
    else
        return Priority::REALTIME;
}

void Thread::SetPriorityCurrentThread(Priority priority)
{
    int nPriority = THREAD_PRIORITY_NORMAL;
    switch (priority)
    {
        case Priority::IDLE:
            nPriority = THREAD_PRIORITY_IDLE;
            break;
        case Priority::LOWEST:
            nPriority = THREAD_PRIORITY_LOWEST;
            break;
        case Priority::LOW:
            nPriority = THREAD_PRIORITY_BELOW_NORMAL;
            break;
        case Priority::NORMAL:
            nPriority = THREAD_PRIORITY_NORMAL;
            break;
        case Priority::HIGH:
            nPriority = THREAD_PRIORITY_ABOVE_NORMAL;
            break;
        case Priority::HIGHEST:
            nPriority = THREAD_PRIORITY_HIGHEST;
            break;
        case Priority::REALTIME:
            nPriority = THREAD_PRIORITY_TIME_CRITICAL;
            break;
    }

    if (!SetThreadPriority(GetCurrentThread(), nPriority))
        throw std::runtime_error("Failed to set the current thread priority!");
}

void Thread::SetPriority(std::thread & thread, Priority priority)
{
    int nPriority = THREAD_PRIORITY_NORMAL;
    switch (priority)
    {
        case Priority::IDLE:
            nPriority = THREAD_PRIORITY_IDLE;
            break;
        case Priority::LOWEST:
            nPriority = THREAD_PRIORITY_LOWEST;
            break;
        case Priority::LOW:
            nPriority = THREAD_PRIORITY_BELOW_NORMAL;
            break;
        case Priority::NORMAL:
            nPriority = THREAD_PRIORITY_NORMAL;
            break;
        case Priority::HIGH:
            nPriority = THREAD_PRIORITY_ABOVE_NORMAL;
            break;
        case Priority::HIGHEST:
            nPriority = THREAD_PRIORITY_HIGHEST;
            break;
        case Priority::REALTIME:
            nPriority = THREAD_PRIORITY_TIME_CRITICAL;
            break;
    }

    if (!SetThreadPriority(static_cast<HANDLE>(thread.native_handle()), nPriority))
        throw std::runtime_error("Failed to set the given thread priority!");
}

#endif

} // namespace core