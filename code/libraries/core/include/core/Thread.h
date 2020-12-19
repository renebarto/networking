#pragma once

#if defined(PLATFORM_WINDOWS)
#pragma warning(disable: 4265)
#pragma warning(disable: 4355)
#endif
#include <future>
#if defined(PLATFORM_WINDOWS)
#pragma warning(default: 4265)
#pragma warning(default: 4355)
#endif

#include <functional>
#include <mutex>
#include <ostream>
#include <thread>
#include "osal/TypeInfo.h"

namespace core {

using ThreadFunction = std::function<void ()>;

enum class ThreadState
{
    NotStarted,
    Running,
    Finished,
    Killed,
};

enum class Priority : uint8_t
{
    IDLE     = 0x00,
    LOWEST   = 0x1F,
    LOW      = 0x3F,
    NORMAL   = 0x7F,
    HIGH     = 0x9F,
    HIGHEST  = 0xBF,
    REALTIME = 0xFF
};

class Thread
{
public:
    Thread();
    Thread(const std::string & name);
    Thread(const Thread &) = delete;
    Thread(ThreadFunction threadFunc);
    Thread(ThreadFunction threadFunc, const std::string & name);
    virtual ~Thread();

    Thread & operator = (const Thread &) = delete;

    void Create(ThreadFunction threadFunc);
    void Destroy();
    bool IsAlive();
    bool IsRunning();
    bool IsFinished();
    bool HasDied();
    const std::string & GetName() const;
    // Important: on Linux, thread must be running for SetName to succeed!
    void SetName(const std::string & name);

    // Wait until thread is in signaled state (thread died)
    void WaitForDeath();

    Priority GetPriority();
    void SetPriority(Priority priority);

    static Priority GetPriorityCurrentThread();
    static Priority GetPriority( std::thread & thread);
    static void SetPriorityCurrentThread(Priority priority);
    static void SetPriority(std::thread & thread, Priority priority);

    void GetResult()
    {
        // Will thrown exception thrown by thread if it threw,
        // or std::future_error when thread was not run
        m_threadResult.get();
    }
    bool HaveResult()
    {
        return m_threadResult.valid();
    }

protected:
    typedef std::recursive_mutex Mutex;
    typedef std::lock_guard<Mutex> Lock;

    Mutex m_threadMutex;
    std::thread m_thread;
    std::string m_name;
    ThreadState m_state;
    std::future<void> m_threadResult;
    void Cleanup();
};

inline std::ostream & operator << (std::ostream & stream, const Thread & thread)
{
    return stream << osal::type(thread) << "Name=" << thread.GetName();
}

} // namespace core
