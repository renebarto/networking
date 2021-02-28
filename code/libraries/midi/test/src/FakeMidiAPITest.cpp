#include "GoogleTest.h"

#include "FakeMidiAPI.h"
#include "midi/MidiCapabilities.h"

namespace midi
{
    
TEST(FakeMidiAPITest, Initialize)
{
    FakeMidiAPI api;

    EXPECT_TRUE(api.Initialize());
    EXPECT_TRUE(api.Initialize());
    api.Uninitialize();
    EXPECT_TRUE(api.Initialize());
}

TEST(FakeMidiAPITest, NotInitialized)
{
    FakeMidiAPI api;

    EXPECT_EQ(std::size_t {0}, api.GetNumInputDevices());
    EXPECT_EQ(std::size_t {0}, api.GetNumOutputDevices());
    MidiCapabilities capabilities;
    EXPECT_FALSE(api.GetCapabilitiesForInputDevice(0, capabilities));
    EXPECT_FALSE(api.GetCapabilitiesForOutputDevice(0, capabilities));
}

TEST(FakeMidiAPITest, GetNumInputDevices)
{
    FakeMidiAPI api;

    EXPECT_TRUE(api.Initialize());
    EXPECT_EQ(std::size_t {1}, api.GetNumInputDevices());
}

TEST(FakeMidiAPITest, GetNumOutputDevices)
{
    FakeMidiAPI api;

    EXPECT_TRUE(api.Initialize());
    EXPECT_EQ(std::size_t {1}, api.GetNumOutputDevices());
}

TEST(FakeMidiAPITest, GetCapabilitiesForInputDevice)
{
    FakeMidiAPI api;

    EXPECT_TRUE(api.Initialize());
    MidiCapabilities capabilities;
    EXPECT_TRUE(api.GetCapabilitiesForInputDevice(0, capabilities));
    EXPECT_EQ(FakeMidiAPI::InputDeviceName, capabilities.Name());
}

TEST(FakeMidiAPITest, GetCapabilitiesForOutputDevice)
{
    FakeMidiAPI api;

    EXPECT_TRUE(api.Initialize());
    MidiCapabilities capabilities;
    EXPECT_TRUE(api.GetCapabilitiesForOutputDevice(0, capabilities));
    EXPECT_EQ(FakeMidiAPI::OutputDeviceName, capabilities.Name());
}

} // namespace midi
