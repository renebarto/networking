#include "GoogleTest.h"

#include <ctime>

#include "osal/Clock.h"

namespace osal {

static const int SecondsPerMinute = 60;
static const int SecondsPerHour = 3600;
static const int SecondsPerDay = 86400;
static const int DaysOfMonth[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

static bool IsLeapYear(int year)
{
    if (year % 4 != 0) return false;
    if (year % 100 != 0) return true;
    return (year % 400) == 0;
}

static time_t mkgmtime(std::tm * tm)
{
    time_t secs = 0;
    for (int y = 1970; y < 1900 + tm->tm_year; ++y)
        secs += (IsLeapYear(y)? 366: 365) * SecondsPerDay;
    for (int m = 0; m < tm->tm_mon; ++m) {
        secs += DaysOfMonth[m] * SecondsPerDay;
        if (m == 1 && IsLeapYear(tm->tm_year)) secs += SecondsPerDay;
    }
    secs += (tm->tm_mday - 1) * SecondsPerDay;
    secs += tm->tm_hour * SecondsPerHour;
    secs += tm->tm_min * SecondsPerMinute;
    secs += tm->tm_sec;
    return secs;
}

class ClockTest
    : public ::testing::Test
{
public:
    virtual void SetUp() {}
    virtual void TearDown() {}
    EpochTime TimeProviderZero() { return EpochTime {0}; }
    EpochTime TimeProviderYear2020()
    {
//TICS -POR#021 We need to initialize for Linux
#if defined (_MSC_VER)
        std::tm timeSpec { };
#else
        std::tm timeSpec { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
#endif
//TICS +POR#021

        timeSpec.tm_sec = 1;
        timeSpec.tm_min = 2;
        timeSpec.tm_hour = 3;
        timeSpec.tm_mday = 4;
        timeSpec.tm_mon = 5;
        timeSpec.tm_year = 120;
        timeSpec.tm_isdst = 1;
        std::time_t timeEpochSeconds = mkgmtime(&timeSpec);
        auto clock = std::chrono::system_clock::from_time_t(timeEpochSeconds);
        return std::chrono::duration_cast<std::chrono::nanoseconds>(clock.time_since_epoch());
    }
};

TEST_F(ClockTest, ConstructWithDefaultTimeProviderReturnsNonZeroTime)
{
    Clock clock;
    EXPECT_NE(EpochTime {0}, clock.CurrentTime());
}

TEST_F(ClockTest, WhenSpecifiedTimeProviderReturningZeroReturnsZero)
{
    Clock clock;
    clock.SetTimeProvider(std::bind(&ClockTest::TimeProviderZero, this));
    EXPECT_EQ(EpochTime {0}, clock.CurrentTime());
}

TEST_F(ClockTest, WhenSpecifiedTimeProviderReturningNonZeroReturnsNonZero)
{
    Clock clock;
    clock.SetTimeProvider(std::bind(&ClockTest::TimeProviderYear2020, this));
    EXPECT_NE(EpochTime {0}, clock.CurrentTime());
}

TEST_F(ClockTest, WhenOffsetSetWithSpecifiedTimeProviderReturningZeroReturnsOffset)
{
    Clock clock;
    const auto offset = std::chrono::nanoseconds(1001001000);
    clock.SetTimeProvider(std::bind(&ClockTest::TimeProviderZero, this));
    clock.SetOffset(offset);
    EXPECT_EQ(offset, clock.CurrentTime());
}

TEST_F(ClockTest, WhenOffsetSetWithSpecifiedTimeProviderReturningZeroEtxTimeStillReturnsActualTime)
{
    Clock clock;
    clock.SetTimeProvider(std::bind(&ClockTest::TimeProviderZero, this));
    EXPECT_NE(EpochTime {0}, clock.EtxTime());
}

TEST_F(ClockTest, ToStringForZeroTimeReturnsEpochDateTime)
{
    const std::string EpochDateTime = "1970-Jan-01 00:00:00.000000";
    EXPECT_EQ(EpochDateTime, Clock::ToString(TimeProviderZero()));
}

TEST_F(ClockTest, ToStringFor2020TimeReturnsCorrectDateTime)
{
    const std::string EpochDateTime = "2020-Jun-04 03:02:01.000000";
    EXPECT_EQ(EpochDateTime, Clock::ToString(TimeProviderYear2020()));
}

TEST_F(ClockTest, SleepFor5SecWhenTimeProviderReturningZero)
{
    Clock clock;
    auto timeInSecondsBefore = clock.CurrentTime();
    std::int64_t sleepTimeSeconds = 5;
    Clock::Sleep(std::chrono::seconds(sleepTimeSeconds));
    auto timeInSecondsAfter = clock.CurrentTime();
    auto timeInMilliSeconds = std::chrono::duration_cast<std::chrono::milliseconds>(timeInSecondsAfter - timeInSecondsBefore);
    std::cout << timeInMilliSeconds.count() << std::endl;
    auto timeInSeconds = std::chrono::duration_cast<std::chrono::seconds>(timeInSecondsAfter - timeInSecondsBefore);
    EXPECT_EQ(sleepTimeSeconds, timeInSeconds.count());
}

} // namespace osal
