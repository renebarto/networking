//------------------------------------------------------------------------------
// Copyright   : Copyright(c) 2009 Koninklijke Philips Electronics N.V.
//
// File        : Clock.cpp
//
// Namespace   : osal
//
// Class       : Clock
//
// Description :
//
//------------------------------------------------------------------------------

#include "osal/Clock.h"

#include <ctime>
#include <iomanip>
#include <sstream>
#include <thread>

namespace osal {

std::chrono::nanoseconds Clock::m_timeOffset(0);

Clock::Clock()
    : m_timeProvider(nullptr)
{
}

Clock::~Clock()
{
}

void
Clock::SetTimeProvider(
    TimeProviderFunc timeProvider)
{
    m_timeProvider = timeProvider;
}

void
Clock::SetOffset(
    std::chrono::microseconds offset)
{
    m_timeOffset = std::chrono::duration_cast<std::chrono::nanoseconds>(offset);
}

// Abstract  : Retrieve the current (as set by the client or NTP protocol) time
//             Used by logging/tracing/...
EpochTime
Clock::CurrentTime() const
{
    EpochTime time {};
    // Don't TRACE!!!! Tracing uses this function!!
    // Catch error in catch all catch method
    try
    {
        if (m_timeProvider != nullptr)
            time = m_timeProvider();
        else
            time = std::chrono::system_clock::now().time_since_epoch();
        time += m_timeOffset;
    }
    catch (...)
    {
        time = EpochTime(0);
    }
    return time;
}

// Abstract  : Retrieve the EtxTime.
//             This is the time of the ETX hardware clock.
//             The ETX clock is used by the operating system for timers etc
//             Hence it may not be changed while FDC.out is running;
//             this would cause incorrect timeouts or timers that expire prematurely.
//
//             This is not the time set by the client:
//             Hence it may not be used for for display or logging
EpochTime
Clock::EtxTime() const
{
    EpochTime time {};
    try
    {
        time = std::chrono::high_resolution_clock::now().time_since_epoch();
    }
    catch (std::exception &)
    {
        time = EpochTime(0);
    }
    return time;
}

//
// Abstract  : Retrieve the current (as set by the client or NTP protocol) time in Unix time format;
//             the number of microseconds that have elapsed since 00:00:00 Coordinated Universal Time (UTC), Thursday, 1 January 1970
//
// Pre       :
//
// Post      :
//
// Exceptions:
//
std::chrono::microseconds
Clock::UnixTime() const
{
    return std::chrono::duration_cast<std::chrono::microseconds>(CurrentTime());
}

// Abstract  : Use nanosleep to sleep for the given time duration.
//             Nanosleep is used because usleep will give problems with signals
//             and is therefor not accurate enough.
//
// Pre       :
//
// Post      :
//
// Exceptions:
//
bool
Clock::Sleep(std::chrono::nanoseconds time)
{
#if defined _MSC_VER
    std::this_thread::sleep_for(time);
    return true;
#else
    timespec tmReq;
    auto nanoSeconds = time.count();
    tmReq.tv_sec = static_cast<long>(nanoSeconds / 1000000000);
    tmReq.tv_nsec = static_cast<long>(nanoSeconds % 1000000000);

    // Wait until time expired, nanosleep can be interrupted by signal in which
    // case return value == -1 and errno == EINTR, remaining time is returned.
    // If this happens restart nanosleep with remaining time.
    int status = 0;
    do
    {
        timespec tmRem;
        status = nanosleep(&tmReq, &tmRem);
        tmReq = tmRem;
    }
    while (status == -1 && errno == EINTR);

    return (status != -1);
#endif
}

std::string
Clock::ToString() const
{
    return ToString(CurrentTime());
}

std::string
Clock::ToString(const EpochTime & time)
{
    std::string timeString;
    static const std::string monthNames[] = {
        "Jan",
        "Feb",
        "Mar",
        "Apr",
        "May",
        "Jun",
        "Jul",
        "Aug",
        "Sep",
        "Oct",
        "Nov",
        "Dec"
    };
    try
    {
        // Format 1970-Jan-01 00:00:00.000000
        std::chrono::system_clock::duration clockDuration = std::chrono::duration_cast<std::chrono::system_clock::duration>(time);
        std::chrono::time_point<std::chrono::system_clock, std::chrono::system_clock::duration> timePoint(clockDuration);
        std::chrono::system_clock::time_point clockTimePoint(timePoint);
        std::time_t now = std::chrono::system_clock::to_time_t(clockTimePoint);
        std::tm * gmtTime = std::gmtime(&now);
        auto nanoSeconds = time.count() % 1000000000;
        std::ostringstream str;
        str << std::setw(4) << std::setfill('0') << (gmtTime->tm_year + 1900) << "-"
            << std::setw(2) << std::setfill('0') << monthNames[gmtTime->tm_mon] << "-"
            << std::setw(2) << std::setfill('0') << gmtTime->tm_mday << " "
            << std::setw(2) << std::setfill('0') << gmtTime->tm_hour << ":"
            << std::setw(2) << std::setfill('0') << gmtTime->tm_min << ":"
            << std::setw(2) << std::setfill('0') << gmtTime->tm_sec << "."
            << std::setw(6) << std::setfill('0') << (nanoSeconds + 500) / 1000;
        timeString = str.str();
    }
    catch (std::exception &)
    {
    }

    return timeString;
}

} // namespace osal
