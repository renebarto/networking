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

#if defined(PLATFORM_WINDOWS)

#include <map>
#include <string>
#include <vector>
#include "midi/IMidiAPI.h"

#include <windows.h>

namespace midi {

enum class MidiMessage : unsigned int {
    MidiInOpen = MIM_OPEN,
    MidiInClose = MIM_CLOSE,
    MidiInData = MIM_DATA,
    MidiInLongData = MIM_LONGDATA,
    MidiInError = MIM_ERROR,
    MidiInLongError = MIM_LONGERROR,
    MidiOutOpen = MOM_OPEN,
    MidiOutClose = MOM_CLOSE,
    MidiOutDone = MOM_DONE,
    MidiInMoreData = MIM_MOREDATA,
    MidiOutPositionCallback = MOM_POSITIONCB,
};

std::ostream & operator << (std::ostream & stream, MidiMessage message);

extern const HMIDIIN InvalidMidiInHandle;
extern const HMIDIOUT InvalidMidiOutHandle;

class MidiAPIWindows
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
    MidiAPIWindows();
    ~MidiAPIWindows();

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

    std::size_t FindInputDevice(const std::string & name) const;
    std::size_t FindOutputDevice(const std::string & name) const;
    bool GetMidiInDevCaps(std::size_t index, MIDIINCAPS & caps) const;
    bool GetMidiOutDevCaps(std::size_t index, MIDIOUTCAPS & caps) const;
    HMIDIIN OpenMidiInputDevice(std::size_t index, IMidiInDevice * instance);
    void CloseMidiInputDevice(HMIDIIN handle);
    bool StartMidiInputDevice(HMIDIIN handle);
    bool StopMidiInputDevice(HMIDIIN handle);
    HMIDIOUT OpenMidiOutputDevice(std::size_t index, IMidiOutDevice * instance);
    void CloseMidiOutputDevice(HMIDIOUT handle);
    bool SendMidiOutMessage(HMIDIOUT handle, std::uint32_t msg);

    std::string GetMidiError(MMRESULT errorCode) const;

    static void MidiInProc(
        HMIDIIN   handle,
        UINT      msg,
        DWORD_PTR instance,
        DWORD_PTR param1,
        DWORD_PTR param2);
    static void MidiOutProc(
        HMIDIOUT  handle,
        UINT      msg,
        DWORD_PTR instance,
        DWORD_PTR param1,
        DWORD_PTR param2);
};

} // namespace midi

#endif