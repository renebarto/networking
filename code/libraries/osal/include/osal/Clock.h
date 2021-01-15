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
        std::chrono::nanoseconds offset);

    static bool
    SetCurrentTime(
        const EpochTime & newTime);

    EpochTime
    CurrentTime() const;       // typically used by logging, tracing, ...

    EpochTime
    EtxTime() const;           // typically used by SW if absolute time is not important

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
