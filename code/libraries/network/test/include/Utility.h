#pragma once

#include "core/Thread.h"

namespace network {

class BoolReturnThread
    : public core::Thread
{
private:
    std::function<bool ()> m_threadFunc;
    bool m_result;

public:
    BoolReturnThread(std::function<bool ()> threadFunc)
        : Thread()
        , m_threadFunc(threadFunc)
        , m_result()
    {
        Create(std::bind(&BoolReturnThread::ThreadFunc, this));
    }

    bool GetResult() { return m_result; }
    void SetResult(bool value) { m_result = value; }

    void ThreadFunc()
    {
        SetResult(m_threadFunc());
    }
};

extern const std::uint16_t TestPort;

} // namespace network