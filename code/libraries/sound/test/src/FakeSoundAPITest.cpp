#include "GoogleTest.h"

#include "FakeSoundAPI.h"

namespace sound
{
    
TEST(FakeSoundAPITest, Initialize)
{
    FakeSoundAPI api;

    EXPECT_TRUE(api.Initialize(""));
    EXPECT_TRUE(api.IsInitialized());
    EXPECT_TRUE(api.Initialize(""));
    EXPECT_TRUE(api.IsInitialized());
    api.Uninitialize();
    EXPECT_FALSE(api.IsInitialized());
    EXPECT_TRUE(api.Initialize(""));
    EXPECT_TRUE(api.IsInitialized());
}

TEST(FakeSoundAPITest, NotInitialized)
{
    FakeSoundAPI api;

    EXPECT_FALSE(api.IsInitialized());
}

} // namespace sound
