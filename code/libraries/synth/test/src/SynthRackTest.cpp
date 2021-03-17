#include "GoogleTest.h"

#include "synth/SynthRack.h"

namespace synth
{
    
TEST(SynthRackTest, Initialize)
{
    SynthRack api;

    EXPECT_TRUE(api.Initialize(""));
    EXPECT_TRUE(api.IsInitialized());
    EXPECT_TRUE(api.Initialize(""));
    EXPECT_TRUE(api.IsInitialized());
    api.Uninitialize();
    EXPECT_FALSE(api.IsInitialized());
    EXPECT_TRUE(api.Initialize(""));
    EXPECT_TRUE(api.IsInitialized());
}

TEST(SynthRackTest, NotInitialized)
{
    SynthRack api;

    EXPECT_FALSE(api.IsInitialized());
}

} // namespace synth
