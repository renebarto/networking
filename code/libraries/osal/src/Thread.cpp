#include "osal/Thread.h"

#include <map>
#include <sstream>

//TICS -POR#021 Platform dependent
#if defined(PLATFORM_LINUX) || defined(PLATFORM_LINUX_RPI)
#include  <pthread.h>
#else
#if _MSC_VER > 1900 // Versions after VS 2015
#pragma warning(disable: 5039) //TICS !POR#018 !POR#037
#endif
#include <windows.h>
#if _MSC_VER > 1900 // Versions after VS 2015
#pragma warning(default: 5039) //TICS !POR#018 !POR#037
#endif
//TICS +POR#021

#undef min
#endif

namespace osal {

using ThreadLookupMap = std::map<std::thread::id, std::string>;
static ThreadLookupMap & ThreadNameLookupMap()
{
    static std::map<std::thread::id, std::string> lookupThreadMap;
    return lookupThreadMap;
}

static void AddThreadName(std::thread::id threadId, const std::string & threadName)
{
    auto & lookupMap = ThreadNameLookupMap();
    lookupMap[threadId] = threadName;
}

std::string GetThreadNameSelf()
{
    auto & lookupMap = ThreadNameLookupMap();
    auto id = std::this_thread::get_id();
    auto it = lookupMap.find(id);
    if (it != lookupMap.end())
    {
        return it->second;
    }
    std::ostringstream stream;
    stream << "Thread #" << id;
    return stream.str();
}

//TICS -POR#021 Platform dependent
#if defined(PLATFORM_LINUX) || defined(PLATFORM_LINUX_RPI)

static void SetThreadName(std::thread::native_handle_type nativeHandle, const std::thread::id & id, const std::string & name)
{
    if (!name.empty())
    {
        pthread_setname_np(nativeHandle, name.c_str());
        AddThreadName(id, name);
    }
}

void SetThreadNameSelf(const std::string & name)
{
    SetThreadName(pthread_self(), std::this_thread::get_id(), name);
}

void SetThreadName(std::thread & thread, const std::string & name)
{
    SetThreadName(thread.native_handle(), thread.get_id(), name);
}

bool IsThreadSelf(std::thread & thread)
{
    return thread.native_handle() == pthread_self();
}

bool IsThreadAlive(std::thread & thread)
{
    return thread.native_handle() != 0ul;
}

ThreadPriority GetThreadPrioritySelf()
{
    int policy;
    struct sched_param sched;
    int result = pthread_getschedparam(pthread_self(), &policy, &sched);
    if (result != 0)
        throw std::runtime_error("Failed to get the current thread priority!");
    if ((policy == SCHED_FIFO) || (policy == SCHED_RR))
    {
        if (sched.sched_priority < 15)
            return ThreadPriority::IDLE;
        else if (sched.sched_priority < 30)
            return ThreadPriority::LOWEST;
        else if (sched.sched_priority < 50)
            return ThreadPriority::LOW;
        else if (sched.sched_priority < 70)
            return ThreadPriority::NORMAL;
        else if (sched.sched_priority < 85)
            return ThreadPriority::HIGH;
        else if (sched.sched_priority < 99)
            return ThreadPriority::HIGHEST;
        else
            return ThreadPriority::REALTIME;
    } else
        return ThreadPriority::NORMAL;
}

ThreadPriority GetThreadPriority(std::thread & thread)
{
    int policy;
    struct sched_param sched;
    int result = pthread_getschedparam(thread.native_handle(), &policy, &sched);
    if (result != 0)
        throw std::runtime_error("Failed to get the given thread priority!");
    if ((policy == SCHED_FIFO) || (policy == SCHED_RR))
    {
        if (sched.sched_priority < 15)
            return ThreadPriority::IDLE;
        else if (sched.sched_priority < 30)
            return ThreadPriority::LOWEST;
        else if (sched.sched_priority < 50)
            return ThreadPriority::LOW;
        else if (sched.sched_priority < 70)
            return ThreadPriority::NORMAL;
        else if (sched.sched_priority < 85)
            return ThreadPriority::HIGH;
        else if (sched.sched_priority < 99)
            return ThreadPriority::HIGHEST;
        else
            return ThreadPriority::REALTIME;
    } else
        return ThreadPriority::NORMAL;
}

void SetThreadPrioritySelf(ThreadPriority priority)
{
    int policy = SCHED_RR;
    struct sched_param sched;
    sched.sched_priority = 50;
    switch (priority)
    {
        case ThreadPriority::IDLE:
            sched.sched_priority = 1;
            break;
        case ThreadPriority::LOWEST:
            sched.sched_priority = 15;
            break;
        case ThreadPriority::LOW:
            sched.sched_priority = 30;
            break;
        case ThreadPriority::NORMAL:
            sched.sched_priority = 50;
            break;
        case ThreadPriority::HIGH:
            sched.sched_priority = 70;
            break;
        case ThreadPriority::HIGHEST:
            sched.sched_priority = 85;
            break;
        case ThreadPriority::REALTIME:
            sched.sched_priority = 99;
            break;
        default:
            // Do no change priority
            break;
    }

    int result = pthread_setschedparam(pthread_self(), policy, &sched);
    if (result != 0)
        throw std::runtime_error("Failed to set the current thread priority!");
}

void SetThreadPriority(std::thread & thread, ThreadPriority priority)
{
    int policy = SCHED_RR;
    struct sched_param sched;
    sched.sched_priority = 50;
    switch (priority)
    {
        case ThreadPriority::IDLE:
            sched.sched_priority = 1;
            break;
        case ThreadPriority::LOWEST:
            sched.sched_priority = 15;
            break;
        case ThreadPriority::LOW:
            sched.sched_priority = 30;
            break;
        case ThreadPriority::NORMAL:
            sched.sched_priority = 50;
            break;
        case ThreadPriority::HIGH:
            sched.sched_priority = 70;
            break;
        case ThreadPriority::HIGHEST:
            sched.sched_priority = 85;
            break;
        case ThreadPriority::REALTIME:
            sched.sched_priority = 99;
            break;
        default:
            // Do no change priority
            break;
    }

    int result = pthread_setschedparam(thread.native_handle(), policy, &sched);
    if (result != 0)
        throw std::runtime_error("Failed to set the given thread priority!");
}

#elif defined(PLATFORM_WINDOWS)

static const DWORD MS_VC_EXCEPTION = 0x406D1388;
#pragma pack(push,8)
typedef struct
{
	DWORD dwType; // Must be 0x1000.
	LPCSTR szName; // Pointer to name (in user addr space).
	DWORD dwThreadID; // Thread ID (-1=caller thread).
	DWORD dwFlags; // Reserved for future use, must be zero.
} THREADNAME_INFO;
#pragma pack(pop)

static void RaiseException(THREADNAME_INFO & info)
{
#pragma warning(push)
#pragma warning(disable: 6320 6322)
    __try
    {
        ::RaiseException(MS_VC_EXCEPTION, 0, sizeof(info) / sizeof(ULONG_PTR), (ULONG_PTR*)&info);
    }
    __except (EXCEPTION_EXECUTE_HANDLER)
    {
    }
#pragma warning(pop)
}

static void SetThreadName(DWORD threadID, const std::thread::id & id, const std::string & name)
{
    if (!name.empty())
    {
        THREADNAME_INFO info;
        info.dwType = 0x1000;
        info.szName = name.c_str();
        info.dwThreadID = threadID;
        info.dwFlags = 0;

        RaiseException(info);
        AddThreadName(id, name);
    }
}

void SetThreadNameSelf(const std::string & name)
{
    SetThreadName(::GetCurrentThreadId(), std::this_thread::get_id(), name);
}

void SetThreadName(std::thread & thread, const std::string & name)
{
    auto threadHandle = static_cast<HANDLE>(thread.native_handle());
    DWORD threadID = ::GetThreadId(threadHandle);

    SetThreadName(threadID, thread.get_id(), name);
}

bool IsThreadSelf(std::thread & thread)
{
    // This is probably portable
    return thread.get_id() == std::this_thread::get_id();
}

bool IsThreadAlive(std::thread & thread)
{
    return thread.native_handle() != nullptr;
}

ThreadPriority GetThreadPrioritySelf()
{
    int priority = ::GetThreadPriority(::GetCurrentThread());
    if (priority == THREAD_PRIORITY_ERROR_RETURN)
        throw std::runtime_error("Failed to get the current thread priority!");
    if (priority < THREAD_PRIORITY_LOWEST)
        return ThreadPriority::IDLE;
    else if (priority < THREAD_PRIORITY_BELOW_NORMAL)
        return ThreadPriority::LOWEST;
    else if (priority < THREAD_PRIORITY_NORMAL)
        return ThreadPriority::LOW;
    else if (priority < THREAD_PRIORITY_ABOVE_NORMAL)
        return ThreadPriority::NORMAL;
    else if (priority < THREAD_PRIORITY_HIGHEST)
        return ThreadPriority::HIGH;
    else if (priority < THREAD_PRIORITY_TIME_CRITICAL)
        return ThreadPriority::HIGHEST;
    else
        return ThreadPriority::REALTIME;
}

ThreadPriority GetThreadPriority(std::thread & thread)
{
    int priority = ::GetThreadPriority(static_cast<HANDLE>(thread.native_handle()));
    if (priority == THREAD_PRIORITY_ERROR_RETURN)
        throw std::runtime_error("Failed to get the given thread priority!");
    if (priority < THREAD_PRIORITY_LOWEST)
        return ThreadPriority::IDLE;
    else if (priority < THREAD_PRIORITY_BELOW_NORMAL)
        return ThreadPriority::LOWEST;
    else if (priority < THREAD_PRIORITY_NORMAL)
        return ThreadPriority::LOW;
    else if (priority < THREAD_PRIORITY_ABOVE_NORMAL)
        return ThreadPriority::NORMAL;
    else if (priority < THREAD_PRIORITY_HIGHEST)
        return ThreadPriority::HIGH;
    else if (priority < THREAD_PRIORITY_TIME_CRITICAL)
        return ThreadPriority::HIGHEST;
    else
        return ThreadPriority::REALTIME;
}

void SetThreadPrioritySelf(ThreadPriority priority)
{
    int nPriority = THREAD_PRIORITY_NORMAL;
    switch (priority)
    {
        case ThreadPriority::IDLE:
            nPriority = THREAD_PRIORITY_IDLE;
            break;
        case ThreadPriority::LOWEST:
            nPriority = THREAD_PRIORITY_LOWEST;
            break;
        case ThreadPriority::LOW:
            nPriority = THREAD_PRIORITY_BELOW_NORMAL;
            break;
        case ThreadPriority::NORMAL:
            nPriority = THREAD_PRIORITY_NORMAL;
            break;
        case ThreadPriority::HIGH:
            nPriority = THREAD_PRIORITY_ABOVE_NORMAL;
            break;
        case ThreadPriority::HIGHEST:
            nPriority = THREAD_PRIORITY_HIGHEST;
            break;
        case ThreadPriority::REALTIME:
            nPriority = THREAD_PRIORITY_TIME_CRITICAL;
            break;
    }

    if (!::SetThreadPriority(::GetCurrentThread(), nPriority))
        throw std::runtime_error("Failed to set the current thread priority!");
}

void SetThreadPriority(std::thread & thread, ThreadPriority priority)
{
    int nPriority = THREAD_PRIORITY_NORMAL;
    switch (priority)
    {
        case ThreadPriority::IDLE:
            nPriority = THREAD_PRIORITY_IDLE;
            break;
        case ThreadPriority::LOWEST:
            nPriority = THREAD_PRIORITY_LOWEST;
            break;
        case ThreadPriority::LOW:
            nPriority = THREAD_PRIORITY_BELOW_NORMAL;
            break;
        case ThreadPriority::NORMAL:
            nPriority = THREAD_PRIORITY_NORMAL;
            break;
        case ThreadPriority::HIGH:
            nPriority = THREAD_PRIORITY_ABOVE_NORMAL;
            break;
        case ThreadPriority::HIGHEST:
            nPriority = THREAD_PRIORITY_HIGHEST;
            break;
        case ThreadPriority::REALTIME:
            nPriority = THREAD_PRIORITY_TIME_CRITICAL;
            break;
    }

    if (!::SetThreadPriority(static_cast<HANDLE>(thread.native_handle()), nPriority))
        throw std::runtime_error("Failed to set the given thread priority!");
}

#else

#error Unsupported platform

#endif
//TICS +POR#021

} // namespace osal