//------------------------------------------------------------------------------
// Copyright   : Copyright(c) 2020, Koninklijke Philips Electronics N.V.
//
// File        : SemaphoreTest.cpp
//
// Namespace   : osal
//
// Class       : SemaphoreTest
//
// Description :
//
//------------------------------------------------------------------------------

#include "GoogleTest.h"

#include "osal/Semaphore.h"

#include <thread>
#if defined(PLATFORM_WINDOWS)
#pragma warning(disable: 4265) //TICS !POR#018 !POR#037
#pragma warning(disable: 4355) //TICS !POR#018 !POR#037 Suppress compilation warnings for Windows only
#endif
#include <future>
#if defined(PLATFORM_WINDOWS)
#pragma warning(default: 4265) //TICS !POR#018 !POR#037
#pragma warning(default: 4355) //TICS !POR#018 !POR#037 Suppress compilation warnings for Windows only
#endif
//TICS +POR#021

namespace osal {

class SemaphoreAccessor
    : public osal::Semaphore
{
public:
    int get_value() {
        return this->m_value;
    }
};


class SemaphoreTest
    : public ::testing::Test
{
public:
    virtual void SetUp() {}
    virtual void TearDown() {}
};

TEST_F(SemaphoreTest, Create) {
    SemaphoreAccessor sem;
    EXPECT_EQ(0, sem.get_value());
    std::ostringstream stream;
    stream << sem;
    EXPECT_EQ("osal::Semaphore Value=0", stream.str());
}

TEST_F(SemaphoreTest, Notify) {
    SemaphoreAccessor sem;
    sem.Post();
    EXPECT_EQ(1, sem.get_value());
    std::ostringstream stream;
    stream << sem;
    EXPECT_EQ("osal::Semaphore Value=1", stream.str());
}

TEST_F(SemaphoreTest, NotifyAhead) {
    SemaphoreAccessor sem;
    sem.Post();
    EXPECT_EQ(1, sem.get_value());
    
    auto asyncCall = std::async(std::launch::async, [&sem]() -> bool {
        sem.Wait();
        return true;
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    asyncCall.wait();
    EXPECT_EQ(0, sem.get_value());
    EXPECT_TRUE(asyncCall.get());
}

TEST_F(SemaphoreTest, NotifyAheadMultiple) {
    const int count = 10;
    SemaphoreAccessor sem;

    for (int i = 0; i < count; i++) {
        sem.Post();
    }
    EXPECT_EQ(count, sem.get_value());
    
    auto asyncCall = std::async(std::launch::async, [&sem, &count]() -> bool {
        for (int i = 0; i < count; i++) { 
            sem.Wait();
        }
        return true;
    });

    asyncCall.wait();
    EXPECT_EQ(0, sem.get_value());
    EXPECT_TRUE(asyncCall.get());
}

TEST_F(SemaphoreTest, Wait) {
    SemaphoreAccessor sem;

    auto asyncCall = std::async(std::launch::async, [&sem]() -> bool {
        sem.Wait();
        return true;
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    sem.Post();
    asyncCall.wait();
    EXPECT_EQ(0, sem.get_value());
    EXPECT_TRUE(asyncCall.get());
}


TEST_F(SemaphoreTest, WaitFor) {
    SemaphoreAccessor sem;

    auto asyncCall = std::async(std::launch::async, [&sem]() -> bool {
        return sem.WaitFor(std::chrono::milliseconds(1000));
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    sem.Post();

    asyncCall.wait();
    EXPECT_EQ(0, sem.get_value());
    EXPECT_TRUE(asyncCall.get());
}

TEST_F(SemaphoreTest, WaitForTimeout) {
    SemaphoreAccessor sem;

    auto asyncCall = std::async(std::launch::async, [&sem]() -> bool {
        return sem.WaitFor(std::chrono::milliseconds(50));
    });

    asyncCall.wait();
    EXPECT_EQ(0, sem.get_value());
    EXPECT_FALSE(asyncCall.get());
}

TEST_F(SemaphoreTest, WaitUntil) {
    SemaphoreAccessor sem;

    auto asyncCall = std::async(std::launch::async, [&sem]() -> bool {
        auto deadline = std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(1000);
        return sem.WaitUntil(deadline);
    });

    std::this_thread::sleep_for(std::chrono::milliseconds(10));
    sem.Post();

    asyncCall.wait();
    EXPECT_EQ(0, sem.get_value());
    EXPECT_TRUE(asyncCall.get());
}

TEST_F(SemaphoreTest, WaitUntilTimeout) {
    SemaphoreAccessor sem;

    auto asyncCall = std::async(std::launch::async, [&sem]() -> bool {
        auto deadline = std::chrono::high_resolution_clock::now() + std::chrono::milliseconds(50);
        return sem.WaitUntil(deadline);
    });

    asyncCall.wait();
    EXPECT_EQ(0, sem.get_value());
    EXPECT_FALSE(asyncCall.get());
}

} // namespace osal
