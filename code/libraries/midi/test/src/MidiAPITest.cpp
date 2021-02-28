#include "GoogleTest.h"

#include "midi/IMidiAPI.h"
#include "midi/MidiCapabilities.h"

namespace midi
{
    
TEST(MidiAPITest, Initialize)
{
    auto api = CreateAPI();

    EXPECT_TRUE(api->Initialize());
    EXPECT_TRUE(api->Initialize());
    api->Uninitialize();
    EXPECT_TRUE(api->Initialize());
}

TEST(MidiAPITest, NotInitialized)
{
    auto api = CreateAPI();

    EXPECT_EQ(std::size_t {0}, api->GetNumInputDevices());
    EXPECT_EQ(std::size_t {0}, api->GetNumOutputDevices());
    MidiCapabilities capabilities;
    EXPECT_FALSE(api->GetCapabilitiesForInputDevice(0, capabilities));
    EXPECT_FALSE(api->GetCapabilitiesForOutputDevice(0, capabilities));
}

TEST(MidiAPITest, GetCapabilitiesForInputDevices)
{
    auto api = CreateAPI();

    EXPECT_TRUE(api->Initialize());
    MidiCapabilities capabilities;
    for (std::size_t i = 0; i < api->GetNumInputDevices(); ++i)
    {
        EXPECT_TRUE(api->GetCapabilitiesForInputDevice(i, capabilities));
    }
}

TEST(MidiAPITest, GetCapabilitiesForOutputDevices)
{
    auto api = CreateAPI();

    EXPECT_TRUE(api->Initialize());
    MidiCapabilities capabilities;
    for (std::size_t i = 0; i < api->GetNumOutputDevices(); ++i)
    {
        EXPECT_TRUE(api->GetCapabilitiesForOutputDevice(i, capabilities));
    }
}

} // namespace midi
