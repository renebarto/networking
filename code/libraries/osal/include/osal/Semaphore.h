//------------------------------------------------------------------------------
// Copyright   : Copyright(c) 2020, Koninklijke Philips Electronics N.V.
//
// File        : Semaphore.h
//
// Namespace   : osal
//
// Class       : Semaphore
//
// Description :
//
//------------------------------------------------------------------------------

#pragma once

#include <chrono>
#include <mutex>
#include <condition_variable>
#include "osal/TypeInfo.h"

namespace osal {

class Semaphore
{
public:
    Semaphore()
        : m_value(0)
    {}
    virtual ~Semaphore() = default;

    void Post() {
        Lock lock(m_mutex);
        m_value++;
        m_condition.notify_one();
    }

    void Wait() {
        Lock lock(m_mutex);
        m_condition.wait(lock, [this]() { return m_value > 0; });
        m_value--;
    }

    template<class Clock, class Period>
    bool WaitFor(const std::chrono::duration<Clock, Period>& timeout) {
        auto deadline = std::chrono::high_resolution_clock::now() + timeout; 
        return WaitUntil(deadline);
    }

    template<class Clock, class Duration>
    bool WaitUntil(const std::chrono::time_point<Clock, Duration>& deadline) {
        Lock lock(m_mutex);
        if (m_condition.wait_until(lock, deadline, [this]() { return m_value > 0; })) {
            m_value--;
            return true;
        }
        return false;
    }

    friend std::ostream & operator << (std::ostream & stream, const Semaphore & semaphore);

protected:
    using Mutex = std::mutex;
    using Lock = std::unique_lock<Mutex>;
    Mutex m_mutex;
    std::condition_variable m_condition;
    int m_value;
};

inline std::ostream & operator << (std::ostream & stream, const Semaphore & semaphore)
{
    return stream << osal::type(semaphore) << " Value=" << semaphore.m_value;
}

} // namespace osal
