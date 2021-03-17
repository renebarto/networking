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

#include "midi/IMidiOutDevice.h"
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

static void ComposeMidiOutMessage(const MidiEvent & event, std::uint32_t & msg)
{
    std::uint8_t byte1 = static_cast<std::uint8_t>(event.type);
    std::uint8_t byte2 = static_cast<std::uint8_t>(event.key);
    std::uint8_t byte3 = static_cast<std::uint8_t>(event.velocity);
    msg = static_cast<std::uint32_t>(byte1 | (byte2 << 8) | (byte3 << 16));
}

class MidiOutDeviceWindows
    : public IMidiOutDevice
{
private:
    MidiAPIWindows * m_api;
    HMIDIOUT m_handle;

public:
    MidiOutDeviceWindows(IMidiAPI * api)
        : m_api(dynamic_cast<MidiAPIWindows *>(api))
        , m_handle(InvalidMidiOutHandle)
    {}
    ~MidiOutDeviceWindows()
    {
        if (IsOpen())
            Close();
    }

    bool Open(std::size_t index) override
    {
        bool result {};
        SCOPEDTRACE(
            nullptr, 
            [&] () { return utility::FormatString("result={}", result); });
        m_handle = m_api->OpenMidiOutputDevice(index, this);
        result = (m_handle != InvalidMidiOutHandle);
        return result;
    }
    bool IsOpen() const override
    {
        bool result = (m_handle != InvalidMidiOutHandle);
        SCOPEDTRACE(
            nullptr, 
            [&] () { return utility::FormatString("result={}", result); });
        return result;
    }
    void Close() override
    {
        SCOPEDTRACE(nullptr, nullptr);
        if (m_handle != InvalidMidiOutHandle)
        {
            m_api->CloseMidiOutputDevice(m_handle);
            m_handle = InvalidMidiOutHandle;
        }
    }

    void SendMidiOutEvent(std::uint32_t msg) override
    {
        TraceInfo(__FILE__, __LINE__, __func__, "Send MIDI event: msg={,8:X8}", msg);
        m_api->SendMidiOutMessage(m_handle, msg);
    }

    void SendMidiOutEvent(const MidiEvent & event) override
    {
        TraceInfo(__FILE__, __LINE__, __func__, "event={}, key={}, velocity={}, timestamp={}", 
            event.type, event.key, event.velocity, event.timestamp);
        std::uint32_t msg = event.MakeParam1();
        SendMidiOutEvent(msg);
    }
};

IMidiOutDevicePtr CreateMidiOutDevice(IMidiAPI * api)
{
    SCOPEDTRACE(nullptr, nullptr);
    return std::make_shared<MidiOutDeviceWindows>(api);
}

#elif defined(PLATFORM_LINUX)

using MidiOutHandle = snd_seq_t *;

#endif

} // namespace midi
