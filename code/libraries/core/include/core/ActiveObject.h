#pragma once

#include "tracing/ScopedTracing.h"
#include "tracing/Tracing.h"
#include "core/WorkerThread.h"

namespace core {

class ActiveObject
    : public WorkerThread
{
private:
    using Mutex = std::recursive_mutex;
    using Lock = std::lock_guard<Mutex>;
    std::atomic<bool> m_isDying;
    std::atomic<bool> m_isAlive;

public:
    ActiveObject() = delete;
    ActiveObject(const ActiveObject &) = delete;
    ActiveObject & operator = (const ActiveObject &) = delete;

    ActiveObject(const std::string & name)
        : WorkerThread(name)
        , m_isDying(false)
        , m_isAlive(false)
    {
        SCOPEDTRACE(nullptr, nullptr);
    }
    virtual ~ActiveObject()
    {
        SCOPEDTRACE(nullptr, nullptr);
    }

    void Kill()
    {
        SCOPEDTRACE(nullptr, nullptr);
        if (WorkerThread::IsRunning())
        {
            m_isDying = true;

            TraceMessage(__FILE__, __LINE__, __func__, "Signaling thread to shut down");

            FlushThread();

            TraceMessage(__FILE__, __LINE__, __func__, "Waiting for thread to shut down");

            WorkerThread::WaitForDeath();
            WorkerThread::Destroy();
        }
    }
    void Create()
    {
        SCOPEDTRACE(nullptr, nullptr);
        m_isDying = false;
        WorkerThread::Create();
    }

    bool IsAlive() const
    {
        bool result = m_isAlive;
        SCOPEDTRACE(nullptr, [&] () { return utility::FormatString(std::string("result={}"), result); });
        return result;
    }
    bool IsDying() const
    {
        bool result = m_isDying;
        SCOPEDTRACE(nullptr, [&] () { return utility::FormatString(std::string("result={}"), result); });
        return result;
    }

protected:
    virtual void Thread() override final
    {
        SCOPEDTRACE(nullptr, nullptr);
        try
        {
            m_isAlive = true;

            TraceMessage(__FILE__, __LINE__, __func__, "Thread {} starting", GetName());

            InitThread();
            Run();
            ExitThread();

            TraceMessage(__FILE__, __LINE__, __func__, "Thread {} shutting down", GetName());
        }
        catch (const std::exception & e)
        {
            TraceMessage(__FILE__, __LINE__, __func__, "Thread {}: Exception thown: {}", GetName(), std::string(e.what()));
            m_isAlive = false;
            throw;
        }

        m_isAlive = false;
    }
    virtual void InitThread() {};
    virtual void Run() = 0;
    virtual void ExitThread() {};
    virtual void FlushThread() {};
};

} // namespace core
