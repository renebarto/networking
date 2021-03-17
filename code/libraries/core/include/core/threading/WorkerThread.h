#pragma once

#include <chrono>
#include "osal/ManualEvent.h"
#include "osal/Signal.h"
#include "core/threading/Thread.h"
#include "tracing/Tracing.h"

namespace core {
namespace threading {

class WorkerThread
    : private Thread
{
public:
    WorkerThread() = delete;
    WorkerThread(const WorkerThread &) = delete;
    WorkerThread & operator = (const WorkerThread &) = delete;

    explicit WorkerThread(const std::string & name)
        : core::threading::Thread(name)
        , m_birthEvent()
    {
    }
    virtual ~WorkerThread()
    {
    }

    virtual void Create()
    {
        m_birthEvent.Reset();
        Thread::Create(std::bind(&WorkerThread::ThreadStarter, this));
        WaitForBirth();
    }
    void WaitForBirth()
    {
        m_birthEvent.Wait();
    }
    bool WaitForBirth(std::chrono::nanoseconds timeout)
    {
        return m_birthEvent.Wait(timeout);
    }

    virtual void Thread() = 0;

    using Thread::Destroy;
    using Thread::GetName;
    using Thread::IsRunning;
    using Thread::WaitForDeath;

protected:
    osal::ManualEvent m_birthEvent;
    void ThreadStarter()
    {
        try
        {
            SetSignalMask();
            TraceInfo(__FILE__, __LINE__, __func__, "Thread {} starting", GetName());
            m_birthEvent.Set();
            Thread();
            TraceInfo(__FILE__, __LINE__, __func__, "Thread {} stopping", GetName());
            m_state = ThreadState::Finished;
        }
        catch (const std::exception & e)
        {
            TraceInfo(__FILE__, __LINE__, __func__, "Thread {}: Exception thown: {}", GetName(), std::string(e.what()));
            m_birthEvent.Set();
            throw;
        }
    }

    void SetSignalMask()
    {
        osal::SignalSet signalMaskSet;
        signalMaskSet.clear();
        signalMaskSet.add(SIGTERM);
        signalMaskSet.add(SIGINT);
#if defined(SIGQUIT)
        signalMaskSet.add(SIGQUIT);
#endif
        // if (!osal::Thread::SetSignalMask(signalMaskSet))
        //     throw OSAL::SystemError(__func__, __FILE__, __LINE__, errno,
        //                             "Cannot set signal mask for thread");
    }
};

} // namespace threading
} // namespace core
