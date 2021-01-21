#include "GoogleTest.h"

#include <sstream>
#include "osal/Thread.h"
#include "core/Thread.h"

namespace osal
{
    
TEST(ThreadTest, ByDefaultNoThreadsMapping)
{
    std::ostringstream stream;
    stream << "Thread #" << std::this_thread::get_id();
    std::string text = stream.str();

    EXPECT_EQ(text, GetThreadNameSelf());
}

TEST(ThreadTest, IfTheMainThreadIsNameItIsInTheMap)
{
    SetThreadNameSelf("Main");

    EXPECT_EQ("Main", GetThreadNameSelf());
}

std::string HelperThread()
{
    std::this_thread::sleep_for(std::chrono::seconds(1));
    return GetThreadNameSelf();
}
TEST(ThreadTest, GetNamesOfMultipleThreads)
{
    SetThreadNameSelf("Main");

    const std::string helperThreadName = "HelperThread";
    core::TypedReturnThread<std::string> helperThread(HelperThread, helperThreadName);
    EXPECT_EQ(helperThreadName, helperThread.GetName());

    helperThread.WaitForDeath();
    auto result = helperThread.GetResult();
   
    EXPECT_EQ(helperThreadName, result);
    EXPECT_EQ("Main", GetThreadNameSelf());
}

} // namespace osal
