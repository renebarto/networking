//------------------------------------------------------------------------------
// Copyright   : Copyright(c) 2020, Rene Barto
//
// File        : FakeMidiAPI.cpp
//
// Namespace   : midi
//
// Class       : -
//
// Description : 
//
//------------------------------------------------------------------------------

#include "FakeMidiAPI.h"

#include "midi/MidiCapabilities.h"

namespace midi {

const std::string FakeMidiAPI::InputDeviceName = "ESI KeyControl 25 XT";
const std::string FakeMidiAPI::OutputDeviceName = "Microsoft GS Wavetable Synth";

FakeMidiAPI::FakeMidiAPI()
    : m_inputDeviceMap()
    , m_outputDeviceMap()
    , m_inputDeviceCapsMap()
    , m_outputDeviceCapsMap()
    , m_isInitialized()
{
}

FakeMidiAPI::~FakeMidiAPI()
{
    Uninitialize();
}

bool FakeMidiAPI::Initialize()
{
    if (m_isInitialized)
        return true;

    m_inputDeviceMap.insert(std::make_pair(InputDeviceName, std::size_t {0}));
    m_outputDeviceMap.insert(std::make_pair(OutputDeviceName, std::size_t {0}));
    m_inputDeviceCapsMap.push_back(MidiCapabilities(InputDeviceName));
    m_outputDeviceCapsMap.push_back(MidiCapabilities(OutputDeviceName));
    m_isInitialized = true;
    return true;
}

void FakeMidiAPI::Uninitialize()
{
    m_inputDeviceMap.clear();
    m_outputDeviceMap.clear();
    m_isInitialized = false;
}

std::size_t FakeMidiAPI::GetNumInputDevices() const
{
    return m_inputDeviceMap.size();
}

std::size_t FakeMidiAPI::GetNumOutputDevices() const
{
    return m_outputDeviceMap.size();
}

bool FakeMidiAPI::GetCapabilitiesForInputDevice(std::size_t index, MidiCapabilities & capabilities) const
{
    capabilities = {};
    if (index < m_inputDeviceCapsMap.size())
    {
        capabilities = m_inputDeviceCapsMap[index];
        return true;
    }
    return false;
}

bool FakeMidiAPI::GetCapabilitiesForOutputDevice(std::size_t index, MidiCapabilities & capabilities) const
{
    capabilities = {};
    if (index < m_outputDeviceCapsMap.size())
    {
        capabilities = m_outputDeviceCapsMap[index];
        return true;
    }
    return false;
}

bool FakeMidiAPI::ConnectDevices(IMidiInDevicePtr /*deviceIn*/, IMidiOutDevicePtr /*deviceOut*/)
{
    return false;   
}

bool FakeMidiAPI::DisconnectDevices(IMidiInDevicePtr /*deviceIn*/, IMidiOutDevicePtr /*deviceOut*/)
{
    return false;   
}

IMidiInDevicePtr FakeMidiAPI::OpenInputDevice(std::size_t /*index*/)
{
    return {};    
}

IMidiInDevicePtr FakeMidiAPI::OpenInputDevice(const std::string & /*name*/)
{
    return {};    
}

IMidiOutDevicePtr FakeMidiAPI::OpenOutputDevice(std::size_t /*index*/)
{
    return {};    
}

IMidiOutDevicePtr FakeMidiAPI::OpenOutputDevice(const std::string & /*name*/)
{
    return {};    
}

void FakeMidiAPI::CloseInputDevice(IMidiInDevicePtr /*handle*/)
{
    
}

void FakeMidiAPI::CloseOutputDevice(IMidiOutDevicePtr /*handle*/)
{

}

} // namespace midi
