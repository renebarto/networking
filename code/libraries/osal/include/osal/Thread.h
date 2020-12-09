#pragma once

#include <thread>
#include <string>

#if defined(PLATFORM_LINUX)
#include  <pthread.h>
#else
#pragma warning(disable: 5039)
#include <windows.h>
#pragma warning(default: 5039)

#undef min
#endif

#if defined(PLATFORM_LINUX)

inline void SetThreadName(std::thread & thread, const std::string & name)
{
    if (!name.empty())
    {
        pthread_setname_np(thread.native_handle(), name.c_str());
    }
}

inline bool IsThreadSelf(std::thread & thread)
{
    return thread.native_handle() == pthread_self();
}

inline bool IsThreadAlive(std::thread & thread)
{
    return thread.native_handle() != 0ul;
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

void SetThreadName(std::thread & thread, const std::string & name)
{
    auto threadHandle = static_cast<HANDLE>(thread.native_handle());
    if (!name.empty())
    {
        DWORD threadID = ::GetThreadId(threadHandle);

        THREADNAME_INFO info;
        info.dwType = 0x1000;
        info.szName = name.c_str();
        info.dwThreadID = threadID;
        info.dwFlags = 0;

        RaiseException(info);
    }
}

bool IsThreadSelf(std::thread & thread)
{
    // This is probably portable
    return thread.get_id() == std::this_thread::get_id();
}

inline bool IsThreadAlive(std::thread & thread)
{
    return thread.native_handle() != nullptr;
}

#else

#error Unsupported platform

#endif