#include "GoogleTest.h"

#include "synth/Synth.h"

namespace synth
{
    
TEST(SynthTest, Initialize)
{
    Synth api;

    EXPECT_TRUE(api.Initialize());
    EXPECT_TRUE(api.IsInitialized());
    EXPECT_TRUE(api.Initialize());
    EXPECT_TRUE(api.IsInitialized());
    api.Uninitialize();
    EXPECT_FALSE(api.IsInitialized());
    EXPECT_TRUE(api.Initialize());
    EXPECT_TRUE(api.IsInitialized());
}

TEST(SynthTest, NotInitialized)
{
    Synth api;

    EXPECT_FALSE(api.IsInitialized());
}

} // namespace synth
