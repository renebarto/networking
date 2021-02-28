//------------------------------------------------------------------------------
// Copyright   : Copyright(c) 2020, Rene Barto
//
// File        : MidiDevice.cpp
//
// Namespace   : midi
//
// Class       : -
//
// Description : 
//
//------------------------------------------------------------------------------

#include "tracing/ScopedTracing.h"

#include "midi/IMidiInDevice.h"
#include "midi/IMidiAPI.h"

#if defined(PLATFORM_WINDOWS)

#include <windows.h>
#include "MidiAPIWindows.h"

#elif defined(PLATFORM_LINUX)

#include <alsa/asoundlib.h>

#else

#error Unsupported platform

#endif

namespace midi {

#if defined(PLATFORM_WINDOWS)

class MidiInDeviceWindows
    : public IMidiInDevice
{
private:
    MidiAPIWindows * m_api;
    HMIDIIN m_handle;
    bool m_isStarted;
    MidiEventCallback m_eventCallback;

public:
    MidiInDeviceWindows(IMidiAPI * api)
        : m_api(dynamic_cast<MidiAPIWindows *>(api))
        , m_handle(InvalidMidiInHandle)
        , m_isStarted()
        , m_eventCallback()
    {
        SCOPEDTRACE(
            nullptr, 
            nullptr);
    }
    ~MidiInDeviceWindows()
    {
        SCOPEDTRACE(
            nullptr, 
            nullptr);
        if (IsOpen())
            Close();
    }

    bool Open(std::size_t index) override
    {
        bool result {};
        SCOPEDTRACE(
            nullptr, 
            [&] () { return utility::FormatString("result={}", result); });
        m_handle = m_api->OpenMidiInputDevice(index, this);
        result = (m_handle != InvalidMidiInHandle);
        return result;
    }
    bool IsOpen() const override
    {
        bool result = (m_handle != InvalidMidiInHandle);
        SCOPEDTRACE(
            nullptr, 
            [&] () { return utility::FormatString("result={}", result); });
        return result;
    }
    void Close() override
    {
        SCOPEDTRACE(nullptr, nullptr);
        if (m_handle != InvalidMidiInHandle)
        {
            m_api->CloseMidiInputDevice(m_handle);
            m_handle = InvalidMidiInHandle;
        }
    }

    bool Start() override {
        bool result {};
        SCOPEDTRACE(
            nullptr, 
            [&] () { return utility::FormatString("result={}", result); });

        result = m_api->StartMidiInputDevice(m_handle);

        if (result)
            m_isStarted = true;
        return result;
    }
    bool Stop() override {
        bool result {};
        SCOPEDTRACE(
            nullptr, 
            [&] () { return utility::FormatString("result={}", result); });

        result = m_api->StopMidiInputDevice(m_handle);

        if (result)
            m_isStarted = false;
            
        return result;
    }

    bool IsStarted() override {
        bool result = m_isStarted;
        SCOPEDTRACE(
            nullptr, 
            [&] () { return utility::FormatString("result={}", result); });

            
        return result;
    }

    void OnMidiEvent(std::uint32_t msg, std::uint32_t param1, std::uint32_t param2) override
    {
        MidiMessage midiMessage = static_cast<MidiMessage>(msg);
        TraceMessage(__FILE__, __LINE__, __func__, "Received MIDI event: msg={}, param1={,8:X8}, param2={,8:X8}", midiMessage, param1, param2);
        switch (midiMessage)
        {
            case MidiMessage::MidiInOpen:
            case MidiMessage::MidiInClose:
                break;
            case MidiMessage::MidiInData:
                if (m_eventCallback)
                    m_eventCallback(MidiEvent(param1, param2));
                break;
            default:
                break;
        }
    }

    void SetEventCallback(MidiEventCallback callback) override
    {
        m_eventCallback = callback;
    }
};

IMidiInDevicePtr CreateMidiInDevice(IMidiAPI * api)
{
    SCOPEDTRACE(nullptr, nullptr);
    return std::make_shared<MidiInDeviceWindows>(api);
}

#elif defined(PLATFORM_LINUX)

using MidiInHandle = snd_seq_t *;

#endif

} // namespace midi
