//------------------------------------------------------------------------------
// Copyright   : Copyright(c) 2009 Koninklijke Philips Electronics N.V.
//
// File        : Clock.h
//
// Namespace   : osal
//
// Class       : Clock
//
// Description : Add a time/date interface to set a SW only time mechanism that
//               doesn't influence semaphore time-outs and sleeps
//
//------------------------------------------------------------------------------

#pragma once

#include <chrono>
#include <functional>
#include <string>

namespace osal {

using EpochTime = std::chrono::nanoseconds;

using TimeProviderFunc=std::function<EpochTime(void)>;

class Clock
{
public:
    Clock();
    virtual ~Clock();

    void
    SetTimeProvider(
        TimeProviderFunc timeProvider);

    static void
    SetOffset(
        std::chrono::microseconds offset);

    EpochTime
    CurrentTime() const;       // typically used by logging, tracing, ...

    EpochTime
    EtxTime() const;           // typically used by SW if absolute time is not important

    std::chrono::microseconds
    UnixTime() const;           // typically used by logging

    static bool
    Sleep(
        std::chrono::nanoseconds time);

    std::string
    ToString() const;          // return ISO formatted date string

    static std::string
    ToString(                  // return ISO formatted date string
        const EpochTime & time);

    static std::chrono::nanoseconds m_timeOffset;
    TimeProviderFunc m_timeProvider;
};

inline std::ostream & operator << (std::ostream & stream, const Clock & value)
{
    stream << value.ToString();
    return stream;
}

} // namespace osal
