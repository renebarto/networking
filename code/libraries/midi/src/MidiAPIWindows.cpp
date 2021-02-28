//------------------------------------------------------------------------------
// Copyright   : Copyright(c) 2020, Rene Barto
//
// File        : MidiAPIWindows.cpp
//
// Namespace   : midi
//
// Class       : -
//
// Description : 
//
//------------------------------------------------------------------------------

#if defined(PLATFORM_WINDOWS)

#include "tracing/ScopedTracing.h"
#include "utility/EnumSerialization.h"

#include "midi/IMidiInDevice.h"
#include "midi/IMidiOutDevice.h"
#include "midi/MidiCapabilities.h"
#include "MidiAPIWindows.h"

namespace serialization {

template<>
const utility::BidirectionalMap<midi::MidiEventType, std::string> EnumSerializationMap<midi::MidiEventType>::ConversionMap = 
{
    {midi::MidiEventType::NoteOff, "NoteOff"},
    {midi::MidiEventType::NoteOn, "NoteOn"},
    {midi::MidiEventType::PolyKeyPressure, "PolyKeyPressure"},
    {midi::MidiEventType::ControlChange, "ControlChange"},
    {midi::MidiEventType::ProgramChange, "ProgramChange"},
    {midi::MidiEventType::ChannelPressure, "ChannelPressure"},
    {midi::MidiEventType::PitchBend, "PitchBend"},
};

std::string Serialize(const midi::MidiEvent & event)
{
    std::string result {};
    switch (event.type)
    {
        case midi::MidiEventType::NoteOn:
        case midi::MidiEventType::NoteOff:
            result = utility::FormatString("type={}, channel={}, key={}, velocity={}, timestamp={}", event.type, event.channel, event.key, event.velocity, event.timestamp);
            break;
        case midi::MidiEventType::ControlChange:
            result = utility::FormatString("type={}, channel={}, control={}, value={}, timestamp={}", event.type, event.channel, event.control, event.controlValue, event.timestamp);
            break;
        case midi::MidiEventType::PitchBend:
            result = utility::FormatString("type={}, channel={}, value={}, timestamp={}", event.type, event.channel, event.controlValue, event.timestamp);
            break;
        case midi::MidiEventType::PolyKeyPressure:
            result = utility::FormatString("type={}, channel={}, key={}, value={}, timestamp={}", event.type, event.channel, event.key, event.controlValue, event.timestamp);
            break;
        case midi::MidiEventType::ChannelPressure:
            result = utility::FormatString("type={}, channel={}, value={}, timestamp={}", event.type, event.channel, event.controlValue, event.timestamp);
            break;
        case midi::MidiEventType::ProgramChange:
            result = utility::FormatString("type={}, channel={}, program={}, timestamp={}", event.type, event.channel, event.program, event.timestamp);
            break;
        default:
            result = utility::FormatString("type={}, timestamp={}", event.type, event.timestamp);
    }

    return result;
}

} // namespace serialization

namespace midi {

static const std::size_t InvalidDevice = std::size_t(-1);
const HMIDIIN InvalidMidiInHandle = HMIDIIN(-1);
const HMIDIOUT InvalidMidiOutHandle = HMIDIOUT(-1);

MidiEvent::MidiEvent(std::uint32_t param1, std::uint32_t param2)
    : type(static_cast<MidiEventType>(param1 & 0x000000F0))
    , channel(static_cast<Channel>(param1 & 0x0000000F))
    , key()
    , velocity()
    , control()
    , controlValue()
    , program()
    , pitchBend()
    , timestamp(static_cast<Timestamp>(param2))
{
    switch (type)
    {
        case MidiEventType::NoteOn:
        case MidiEventType::NoteOff:
            key = static_cast<Key>((param1 >> 8) & 0x0000007F);
            velocity = static_cast<Velocity>((param1 >> 16) & 0x0000007F);
            break;
        case MidiEventType::ControlChange:
            control = static_cast<Control>((param1 >> 8) & 0x0000007F);
            controlValue = static_cast<ControlValue>((param1 >> 16) & 0x0000007F);
            break;
        case MidiEventType::ProgramChange:
            program = static_cast<Program>((param1 >> 8) & 0x0000007F);
            break;
        case midi::MidiEventType::PitchBend:
            pitchBend = static_cast<PitchBend>(((param1 >> 8) & 0x0000007F) | (((param1 >> 16) & 0x0000007F) << 7));
            break;
        case MidiEventType::PolyKeyPressure:
            key = static_cast<Key>((param1 >> 8) & 0x0000007F);
            controlValue = static_cast<ControlValue>((param1 >> 16) & 0x0000007F);
            break;
        case MidiEventType::ChannelPressure:
            controlValue = static_cast<ControlValue>((param1 >> 8) & 0x0000007F);
            break;
        default:
            break;
    }
}

MidiEvent::MidiEvent(MidiEventType aType, Channel aChannel, Key aKey, Velocity aVelocity, Timestamp aTimestamp)
    : type(aType)
    , channel(aChannel)
    , key(aKey)
    , velocity(aVelocity)
    , control()
    , controlValue()
    , program()
    , timestamp(aTimestamp)
{
}

// MidiEvent::MidiEvent(MidiEventType aType, Channel aChannel, Control aControl, ControlValue aValue, Timestamp aTimestamp)
//     : type(aType)
//     , channel(aChannel)
//     , key()
//     , velocity()
//     , control(aControl)
//     , controlValue(aValue)
//     , program()
//     , timestamp(aTimestamp)
// {
// }

std::uint32_t MidiEvent::MakeParam1() const
{
    std::uint8_t byte1 = static_cast<std::uint8_t>((static_cast<std::uint8_t>(type) & 0xF0) | (static_cast<std::uint8_t>(channel) & 0x0F));
    std::uint8_t byte2 {};
    std::uint8_t byte3 {};
    switch (type)
    {
        case MidiEventType::NoteOn:
        case MidiEventType::NoteOff:
            byte2 = static_cast<std::uint8_t>(static_cast<std::uint8_t>(key) & 0x7F);
            byte3 = static_cast<std::uint8_t>(static_cast<std::uint8_t>(velocity) & 0x7F);
            break;
        case MidiEventType::ControlChange:
            byte2 = static_cast<std::uint8_t>(static_cast<std::uint8_t>(control) & 0x7F);
            byte3 = static_cast<std::uint8_t>(static_cast<std::uint8_t>(controlValue) & 0x7F);
            break;
        case MidiEventType::ProgramChange:
            byte2 = static_cast<std::uint8_t>(static_cast<std::uint8_t>(program) & 0x7F);
            break;
        case midi::MidiEventType::PitchBend:
            byte2 = static_cast<std::uint8_t>(static_cast<std::uint8_t>(pitchBend) & 0x7F);
            byte3 = static_cast<std::uint8_t>(static_cast<std::uint8_t>(pitchBend >> 7) & 0x7F);
            break;
        case MidiEventType::PolyKeyPressure:
            byte2 = static_cast<std::uint8_t>(static_cast<std::uint8_t>(key) & 0x7F);
            byte3 = static_cast<std::uint8_t>(static_cast<std::uint8_t>(controlValue) & 0x7F);
            break;
        case MidiEventType::ChannelPressure:
            byte2 = static_cast<std::uint8_t>(static_cast<std::uint8_t>(controlValue) & 0x7F);
            break;
        default:
            break;
    }

    return static_cast<std::uint32_t>(byte1 | (byte2 << 8) | (byte3 << 16));
}

std::uint32_t MidiEvent::MakeParam2() const
{
    return static_cast<std::uint32_t>(timestamp);
}

std::ostream & operator << (std::ostream & stream, const MidiEvent & event)
{
    return stream << serialization::Serialize(event);
}

IMidiAPIPtr CreateAPI()
{
    SCOPEDTRACE(nullptr, nullptr);
    return std::make_unique<MidiAPIWindows>();
}

MidiAPIWindows::MidiAPIWindows()
    : m_inputDeviceMap()
    , m_outputDeviceMap()
    , m_inputDeviceCapsMap()
    , m_outputDeviceCapsMap()
    , m_isInitialized()
{
    SCOPEDTRACE(
        nullptr, 
        nullptr);
}

MidiAPIWindows::~MidiAPIWindows()
{
    SCOPEDTRACE(
        nullptr, 
        nullptr);
    Uninitialize();
}

bool MidiAPIWindows::Initialize()
{
    bool result {};
    SCOPEDTRACE(
        nullptr, 
        [&] () { return utility::FormatString("result={}", result); });
    if (m_isInitialized)
    {
        result = true;
        return result;
    }

    std::size_t numInputDevices = static_cast<std::size_t>(midiInGetNumDevs());
    std::size_t numOutputDevices = static_cast<std::size_t>(midiOutGetNumDevs());

    for (std::size_t i = 0; i < numInputDevices; ++i)
    {
        MIDIINCAPS caps;
        result = GetMidiInDevCaps(i, caps);
        if (!result)
            return false;
        MidiCapabilities capabilities(caps.szPname);
        m_inputDeviceMap.insert(std::make_pair(capabilities.Name(), i));
        m_inputDeviceCapsMap.push_back(capabilities);
    }

    for (std::size_t i = 0; i < numOutputDevices; ++i)
    {
        MIDIOUTCAPS caps;
        result = GetMidiOutDevCaps(i, caps);
        if (!result)
            return false;
        MidiCapabilities capabilities(caps.szPname);
        m_outputDeviceMap.insert(std::make_pair(capabilities.Name(), i));
        m_outputDeviceCapsMap.push_back(capabilities);
    }

    m_isInitialized = true;
    result = true;
    return result;
}

void MidiAPIWindows::Uninitialize()
{
    SCOPEDTRACE(
        nullptr, 
        nullptr);
    m_inputDeviceMap.clear();
    m_outputDeviceMap.clear();
    m_isInitialized = false;
}

std::size_t MidiAPIWindows::GetNumInputDevices() const
{
    std::size_t result {};
    SCOPEDTRACE(
        nullptr, 
        [&] () { return utility::FormatString("result={}", result); });
    result = m_inputDeviceMap.size();
    return result;
}

std::size_t MidiAPIWindows::GetNumOutputDevices() const
{
    std::size_t result {};
    SCOPEDTRACE(
        nullptr, 
        [&] () { return utility::FormatString("result={}", result); });
    result = m_outputDeviceMap.size();
    return result;
}

bool MidiAPIWindows::GetCapabilitiesForInputDevice(std::size_t index, MidiCapabilities & capabilities) const
{
    bool result {};
    SCOPEDTRACE(
        nullptr, 
        [&] () { return utility::FormatString("result={}", result); });
    capabilities = {};
    if (index < m_inputDeviceCapsMap.size())
    {
        capabilities = m_inputDeviceCapsMap[index];
        result = true;
    }
    return result;
}

bool MidiAPIWindows::GetCapabilitiesForOutputDevice(std::size_t index, MidiCapabilities & capabilities) const
{
    bool result {};
    SCOPEDTRACE(
        nullptr, 
        [&] () { return utility::FormatString("result={}", result); });
    capabilities = {};
    if (index < m_outputDeviceCapsMap.size())
    {
        capabilities = m_outputDeviceCapsMap[index];
        result = true;
    }
    return result;
}

bool MidiAPIWindows::ConnectDevices(IMidiInDevicePtr /*deviceIn*/, IMidiOutDevicePtr /*deviceOut*/)
{
    bool result {};
    SCOPEDTRACE(
        nullptr, 
        [&] () { return utility::FormatString("result={}", result); });

    return result;   
}

bool MidiAPIWindows::DisconnectDevices(IMidiInDevicePtr /*deviceIn*/, IMidiOutDevicePtr /*deviceOut*/)
{
    bool result {};
    SCOPEDTRACE(
        nullptr, 
        [&] () { return utility::FormatString("result={}", result); });

    return result;   
}

IMidiInDevicePtr MidiAPIWindows::OpenInputDevice(std::size_t index)
{
    SCOPEDTRACE(
        [&] () { return utility::FormatString("index={}", index); },
        nullptr);

    auto device = CreateMidiInDevice(this);
    device->Open(index);
    return device;
}

IMidiInDevicePtr MidiAPIWindows::OpenInputDevice(const std::string & name)
{
    SCOPEDTRACE(
        [&] () { return utility::FormatString("name={}", name); },
        nullptr);

    auto index = FindInputDevice(name);
    if (index != InvalidDevice)
    {
        auto device = CreateMidiInDevice(this);
        if (!device->Open(index))
            return nullptr;
        return device;
    }
    return nullptr;
}

IMidiOutDevicePtr MidiAPIWindows::OpenOutputDevice(std::size_t index)
{
    SCOPEDTRACE(
        [&] () { return utility::FormatString("index={}", index); },
        nullptr);

    auto device = CreateMidiOutDevice(this);
    device->Open(index);
    return device;
}

IMidiOutDevicePtr MidiAPIWindows::OpenOutputDevice(const std::string & name)
{
    SCOPEDTRACE(
        [&] () { return utility::FormatString("name={}", name); },
        nullptr);

    auto index = FindOutputDevice(name);
    if (index != InvalidDevice)
    {
        auto device = CreateMidiOutDevice(this);
        if (!device->Open(index))
            return nullptr;
        return device;
    }
    return nullptr;
}

void MidiAPIWindows::CloseInputDevice(IMidiInDevicePtr handle)
{
    SCOPEDTRACE(
        [&] () { return utility::FormatString("handle={}", handle); },
        nullptr);

    handle->Close();
}

void MidiAPIWindows::CloseOutputDevice(IMidiOutDevicePtr handle)
{
    SCOPEDTRACE(
        [&] () { return utility::FormatString("handle={}", handle); },
        nullptr);

    handle->Close();
}

std::size_t MidiAPIWindows::FindInputDevice(const std::string & name) const
{
    std::size_t result = InvalidDevice;
    SCOPEDTRACE(
        [&] () { return utility::FormatString("name={}", name); },
        [&] () { return utility::FormatString("result={}", result); });

    auto it = m_inputDeviceMap.find(name);
    if (it != m_inputDeviceMap.end())
    {
        result = it->second;
    }
    return result;
}

std::size_t MidiAPIWindows::FindOutputDevice(const std::string & name) const
{
    std::size_t result = InvalidDevice;
    SCOPEDTRACE(
        [&] () { return utility::FormatString("name={}", name); },
        [&] () { return utility::FormatString("result={}", result); });

    auto it = m_outputDeviceMap.find(name);
    if (it != m_outputDeviceMap.end())
    {
        result = it->second;
    }
    return result;
}

bool MidiAPIWindows::GetMidiInDevCaps(std::size_t index, MIDIINCAPS & caps) const
{
    bool result {};
    SCOPEDTRACE(
        [&] () { return utility::FormatString("index={}", index); }, 
        [&] () { return utility::FormatString("result={}", result); });
    MMRESULT midiResult = midiInGetDevCaps(static_cast<UINT>(index), &caps, sizeof(caps));
    if (midiResult != MMSYSERR_NOERROR)
    {
        TraceMessage(__FILE__, __LINE__, __func__, "Could not get midi input device capabilities for {}, error code {}: {}", index, midiResult, GetMidiError(midiResult));
    }
    result = (midiResult == MMSYSERR_NOERROR);
    return result;
}

bool MidiAPIWindows::GetMidiOutDevCaps(std::size_t index, MIDIOUTCAPS & caps) const
{
    bool result {};
    SCOPEDTRACE(
        [&] () { return utility::FormatString("index={}", index); }, 
        [&] () { return utility::FormatString("result={}", result); });
    MMRESULT midiResult = midiOutGetDevCaps(static_cast<UINT>(index), &caps, sizeof(caps));
    if (midiResult != MMSYSERR_NOERROR)
    {
        TraceMessage(__FILE__, __LINE__, __func__, "Could not get midi output device capabilities for {}, error code {}: {}", index, midiResult, GetMidiError(midiResult));
    }
    result = (midiResult == MMSYSERR_NOERROR);
    return result;
}

HMIDIIN MidiAPIWindows::OpenMidiInputDevice(std::size_t index, IMidiInDevice * instance)
{
    HMIDIIN handle = InvalidMidiInHandle;
    SCOPEDTRACE(
        [&] () { return utility::FormatString("index={}", index); }, 
        [&] () { return utility::FormatString("handle={}", handle); });
    MMRESULT midiResult = midiInOpen(&handle, static_cast<UINT>(index), reinterpret_cast<DWORD_PTR>(&MidiAPIWindows::MidiInProc), reinterpret_cast<DWORD_PTR>(instance), CALLBACK_FUNCTION | MIDI_IO_STATUS);
    if (midiResult != MMSYSERR_NOERROR)
    {
        TraceMessage(__FILE__, __LINE__, __func__, "Could not open midi input device {}, error code {}: {}", index, midiResult, GetMidiError(midiResult));
    }
    return handle;
}

void MidiAPIWindows::CloseMidiInputDevice(HMIDIIN handle)
{
    SCOPEDTRACE(
        [&] () { return utility::FormatString("handle={}", handle); }, 
        nullptr);
    MMRESULT midiResult = midiInClose(handle);
    if (midiResult != MMSYSERR_NOERROR)
    {
        TraceMessage(__FILE__, __LINE__, __func__, "Could not close midi input device {}, error code {}: {}", handle, midiResult, GetMidiError(midiResult));
    }
}

bool MidiAPIWindows::StartMidiInputDevice(HMIDIIN handle)
{
    bool result {};
    SCOPEDTRACE(
        [&] () { return utility::FormatString("handle={}", handle); }, 
        [&] () { return utility::FormatString("result={}", result); });
    MMRESULT midiResult = midiInStart(handle);
    if (midiResult != MMSYSERR_NOERROR)
    {
        TraceMessage(__FILE__, __LINE__, __func__, "Could not start midi input device {}, error code {}: {}", handle, midiResult, GetMidiError(midiResult));
    }
    result = (midiResult == MMSYSERR_NOERROR);
    return result;
}

bool MidiAPIWindows::StopMidiInputDevice(HMIDIIN handle)
{
    bool result {};
    SCOPEDTRACE(
        [&] () { return utility::FormatString("handle={}", handle); }, 
        [&] () { return utility::FormatString("result={}", result); });
    MMRESULT midiResult = midiInStop(handle);
    if (midiResult != MMSYSERR_NOERROR)
    {
        TraceMessage(__FILE__, __LINE__, __func__, "Could not stop midi input device {}, error code {}: {}", handle, midiResult, GetMidiError(midiResult));
    }
    result = (midiResult == MMSYSERR_NOERROR);
    return result;
}

HMIDIOUT MidiAPIWindows::OpenMidiOutputDevice(std::size_t index, IMidiOutDevice * instance)
{
    HMIDIOUT handle = InvalidMidiOutHandle;
    SCOPEDTRACE(
        [&] () { return utility::FormatString("index={}", index); }, 
        [&] () { return utility::FormatString("handle={}", handle); });
    MMRESULT midiResult = midiOutOpen(&handle, static_cast<UINT>(index), reinterpret_cast<DWORD_PTR>(&MidiAPIWindows::MidiOutProc), reinterpret_cast<DWORD_PTR>(instance), CALLBACK_FUNCTION);
    if (midiResult != MMSYSERR_NOERROR)
    {
        TraceMessage(__FILE__, __LINE__, __func__, "Could not open midi output device {}, error code {}: {}", index, midiResult, GetMidiError(midiResult));
    }
    return handle;
}

void MidiAPIWindows::CloseMidiOutputDevice(HMIDIOUT handle)
{
    SCOPEDTRACE(
        [&] () { return utility::FormatString("handle={}", handle); }, 
        nullptr);
    MMRESULT midiResult = midiOutClose(handle);
    if (midiResult != MMSYSERR_NOERROR)
    {
        TraceMessage(__FILE__, __LINE__, __func__, "Could not close midi output device {}, error code {}: {}", handle, midiResult, GetMidiError(midiResult));
    }
}

bool MidiAPIWindows::SendMidiOutMessage(HMIDIOUT handle, std::uint32_t msg)
{
    bool result {};
    SCOPEDTRACE(
        [&] () { return utility::FormatString("handle={}", handle); }, 
        [&] () { return utility::FormatString("result={}", result); });
    MMRESULT midiResult = midiOutShortMsg(handle, static_cast<DWORD>(msg));
    if (midiResult != MMSYSERR_NOERROR)
    {
        TraceMessage(__FILE__, __LINE__, __func__, "Could not send midi output message {} to {}, error code {}: {}", msg, handle, midiResult, GetMidiError(midiResult));
    }
    result = (midiResult == MMSYSERR_NOERROR);
    return result;
}

std::string MidiAPIWindows::GetMidiError(MMRESULT errorCode) const
{
    char text[MAXERRORLENGTH];
    midiInGetErrorTextA(errorCode, text, sizeof(text));
    return text;
}

void MidiAPIWindows::MidiInProc(
    HMIDIIN   handle,
    UINT      msg,
    DWORD_PTR instance,
    DWORD_PTR param1,
    DWORD_PTR param2)
{
    IMidiInDevice * device = reinterpret_cast<IMidiInDevice *>(instance);
    TraceMessage(__FILE__, __LINE__, __func__, "Received MIDI IN: handle={}, msg={}, instance={}, param1={}, param2={}", handle, static_cast<MidiMessage>(msg), device, param1, param2);
    if (device)
        device->OnMidiEvent(static_cast<std::uint32_t>(msg), static_cast<std::uint32_t>(param1), static_cast<std::uint32_t>(param2));
}

void MidiAPIWindows::MidiOutProc(
    HMIDIOUT  handle,
    UINT      msg,
    DWORD_PTR instance,
    DWORD_PTR param1,
    DWORD_PTR param2)
{
    IMidiOutDevice * device = reinterpret_cast<IMidiOutDevice *>(instance);
    TraceMessage(__FILE__, __LINE__, __func__, "Received MIDI OUT: handle={}, msg={}, instance={}, param1={}, param2={}", handle, static_cast<MidiMessage>(msg), device, param1, param2);
    // if (device)
    //     device->OnMidiEvent(static_cast<std::uint32_t>(msg), static_cast<std::uint32_t>(param1), static_cast<std::uint32_t>(param2));
}

std::ostream & operator << (std::ostream & stream, MidiMessage message)
{
    return stream << serialization::Serialize(message, "??");
}

} // namespace midi

namespace serialization {

template<>
const utility::BidirectionalMap<midi::MidiMessage, std::string> EnumSerializationMap<midi::MidiMessage>::ConversionMap = 
{
    {midi::MidiMessage::MidiInOpen, "MidiInOpen"},
    {midi::MidiMessage::MidiInClose, "MidiInClose"},
    {midi::MidiMessage::MidiInData, "MidiInData"},
    {midi::MidiMessage::MidiInLongData, "MidiInLongData"},
    {midi::MidiMessage::MidiInError, "MidiInError"},
    {midi::MidiMessage::MidiInLongError, "MidiInLongError"},
    {midi::MidiMessage::MidiOutOpen, "MidiOutOpen"},
    {midi::MidiMessage::MidiOutClose, "MidiOutClose"},
};

} // namespace serialization

#endif

namespace midi {

std::ostream & operator << (std::ostream & stream, MidiEventType type)
{
    return stream << serialization::Serialize(type, "??");
}

} // namespace midi
