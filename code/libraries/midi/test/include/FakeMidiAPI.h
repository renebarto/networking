//------------------------------------------------------------------------------
// Copyright   : Copyright(c) 2020, Rene Barto
//
// File        : MidiAPI.h
//
// Namespace   : midi
//
// Class       : -
//
// Description : 
//
//------------------------------------------------------------------------------

#pragma once

#include <map>
#include <string>
#include <vector>

#include "midi/IMidiAPI.h"

namespace midi {

class FakeMidiAPI
    : public IMidiAPI
{
private:
    using DeviceMap = std::map<std::string, std::size_t>;
    using DeviceCapsMap = std::vector<MidiCapabilities>;
    DeviceMap m_inputDeviceMap;
    DeviceMap m_outputDeviceMap;
    DeviceCapsMap m_inputDeviceCapsMap;
    DeviceCapsMap m_outputDeviceCapsMap;
    bool m_isInitialized;

public:
    static const std::string InputDeviceName;
    static const std::string OutputDeviceName;

    FakeMidiAPI();
    ~FakeMidiAPI();

    bool Initialize() override;
    void Uninitialize() override;
    std::size_t GetNumInputDevices() const override;
    std::size_t GetNumOutputDevices() const override;
    bool GetCapabilitiesForInputDevice(std::size_t index, MidiCapabilities & capabilities) const override;
    bool GetCapabilitiesForOutputDevice(std::size_t index, MidiCapabilities & capabilities) const override;
    bool ConnectDevices(IMidiInDevicePtr deviceIn, IMidiOutDevicePtr deviceOut) override;
    bool DisconnectDevices(IMidiInDevicePtr deviceIn, IMidiOutDevicePtr deviceOut) override;
    IMidiInDevicePtr OpenInputDevice(std::size_t index) override;
    IMidiInDevicePtr OpenInputDevice(const std::string & name) override;
    IMidiOutDevicePtr OpenOutputDevice(std::size_t index) override;
    IMidiOutDevicePtr OpenOutputDevice(const std::string & name) override;
    void CloseInputDevice(IMidiInDevicePtr handle) override;
    void CloseOutputDevice(IMidiOutDevicePtr handle) override;
};

} // namespace midi
