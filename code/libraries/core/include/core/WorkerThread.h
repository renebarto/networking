#pragma once

#include <chrono>
#include "osal/ManualEvent.h"
#include "osal/Signal.h"
#include "core/Thread.h"
#include "tracing/Tracing.h"

namespace core {

class WorkerThread : private Thread
{
public:
    WorkerThread() = delete;
    WorkerThread(const WorkerThread &) = delete;
    WorkerThread & operator = (const WorkerThread &) = delete;

    explicit WorkerThread(const std::string & name)
        : core::Thread(name)
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

    using Thread::IsRunning;

protected:
    osal::ManualEvent m_birthEvent;
    void ThreadStarter()
    {
        try
        {
            SetSignalMask();
            TraceMessage(__FILE__, __LINE__, __func__, "WorkerThread " + GetName() + ": Thread starting");
            m_birthEvent.Set();
            Thread();
            TraceMessage(__FILE__, __LINE__, __func__, "WorkerThread " + GetName() + ": Thread stopping");
            m_state = ThreadState::Finished;
        }
        catch (const std::exception & /*e*/)
        {
            // TraceDebug("WorkerThread " + GetName() + ": Exception thown: " + std::string(e.what()));
            m_birthEvent.Set();
        }
    }

    void SetSignalMask()
    {
        osal::signal::SignalSet signalMaskSet;
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

} // namespace core
