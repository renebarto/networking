#pragma once

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <mutex>
#include "osal/TypeInfo.h"

namespace osal {

class ManualEvent
{
public:
    ManualEvent()
        : m_value(false)
    {
    }
    virtual ~ManualEvent()
    {
    }
    bool GetValue() const
    {
        return m_value;
    }
    void Set()
    {
        Lock lock(m_mutex);
        m_value.store(true);
        m_cond.notify_all();
    }
    void Reset()
    {
        Lock lock(m_mutex);
        m_value.store(false);
    }
    void Wait()
    {
        bool actual = false;
        if (m_value.compare_exchange_strong(actual, false))
        {
            Lock lock(m_mutex);
            m_cond.wait(lock, [&]{return m_value.load();});
        }
    }
    bool Wait(const std::chrono::nanoseconds & timeout)
    {
        bool result = true;
        bool actual = false;
        if (m_value.compare_exchange_strong(actual, false))
        {
            Lock lock(m_mutex);
            result = m_cond.wait_for(lock, timeout, [&]{return m_value.load();});
        }
        return result;
    }

   friend std::ostream & operator << (std::ostream & stream, const ManualEvent & event);

protected:
    std::atomic_bool m_value;
    typedef std::mutex Mutex;
    typedef std::unique_lock<Mutex> Lock;
    Mutex m_mutex;
    std::condition_variable m_cond;
};

inline std::ostream & operator << (std::ostream & stream, const ManualEvent & event)
{
    return stream << osal::type(event) << " Value=" << event.m_value;
}

} // namespace osal
