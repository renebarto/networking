//------------------------------------------------------------------------------
// Copyright   : Copyright(c) 2020, Rene Barto
//
// File        : IMidiAPI.h
//
// Namespace   : midi
//
// Class       : -
//
// Description : 
//
//------------------------------------------------------------------------------

#pragma once

#include <iostream>
#include <memory>

namespace midi {

class MidiCapabilities;
class IMidiInDevice;
class IMidiOutDevice;

using IMidiInDevicePtr = std::shared_ptr<IMidiInDevice>;
using IMidiOutDevicePtr = std::shared_ptr<IMidiOutDevice>;

enum class MidiEventType : std::uint8_t
{
    NoteOff = 0x80,         // 1000cccc 0kkkkkkk 0vvvvvvv
    NoteOn = 0x90,          // 1001cccc 0kkkkkkk 0vvvvvvv
    PolyKeyPressure = 0xA0, // 1010cccc 0kkkkkkk 0vvvvvvv
    ControlChange = 0xB0,   // 1011cccc 0ccccccc 0vvvvvvv
    ProgramChange = 0xC0,   // 1100cccc 0ppppppp
    ChannelPressure = 0xD0, // 1101cccc 0vvvvvvv
    PitchBend = 0xE0,       // 1110cccc 0vvvvvvv 0vvvvvvv
};

std::ostream & operator << (std::ostream & stream, MidiEventType message);

using Channel = std::uint8_t;   // Only bits 0..6 used
// Channel = 0-15
// Channel 0 is referred to as basic channel, which receives Mode messages
using Key = std::uint8_t;       // Only bits 0..6 used
// Key: 0 = C-1
//      12 = C0
//      24 = C1
//      36 = C2
//      48 = C3
//      60 = C4 (middle C)
//      72 = C5
//      84 = C6
//      96 = C7
//      108 = C8
//      120 = C9
// Full keyboard (88 keys) is from A0 (21) to C8 (108)
using Velocity = std::uint8_t;  // Only bits 0..6 used
// Velocity = 0-127 (exponential function)
//      0 = off
//      1 = ppp
//      64 = mf
//      127 = fff
using Control = std::uint8_t;  // Only bits 0..6 used
// Control  0x00 = Bank select MSB
//          0x01 = Modulation sliders
//          0x07 = Volume (8?)
//          0x08 = Balance
//          0x0A = Pan (9?)
//          0x0B = Expression Controller
//          0x20 = Bask select LSB
//          0x44 = Legato on/off
//          0x46 = Sound Controller #1 = Sound Variation
//          0x47 = Sound Controller #2 = Timbre/Harmonic Intensity
//          0x48 = Sound Controller #3 = Release Time
//          0x49 = Sound Controller #4 = Attack Time
//          0x4A = Sound Controller #5 = Brightness
//          0x4B = Sound Controller #6
//          0x4C = Sound Controller #7
//          0x4D = Sound Controller #8
//          0x4E = Sound Controller #9
//          0x4F = Sound Controller #10
//          0x54 = Portamento Controller
//          0x5B = Reverb
//          0x5C = Tremolo Depth
//          0x5D = Chorus Depth
//          0x5E = Detune Depth
//          0x5F = Phase Depth
using ControlValue = std::uint8_t;  // Only bits 0..6 used
using Program = std::uint8_t;  // Only bits 0..6 used
using PitchBend = std::uint16_t;  // Only bits 0..13 used
using Timestamp = std::uint32_t;

// General message - <status> <data> .. <data>
// <status> = 1xxx xxxx
// <data>   = 0xxx xxxx
// Any sequence can be interrupted by System Real-time message
//
// Channel Message  - Channel Voice     - Note On  1001 cccc kkkk kkkk vvvv vvvv
//                                      - Note Off 1000 cccc kkkk kkkk vvvv vvvv
//                  - Channel Mode      - Omni on
//                                      - Omni off
//                                      - Poly
//                                      - Mono
//                 Channel Voice / Mode messages may be repeated without repeating <status> byte, the same status will used until the next status byte is received (Running Status)
// System Message   - System Common     - 1xxx xxxx 0xxx xxxx
//                  - System Real-time  - 1xxx xxxx
//                  - System Exclusive  - 1xxx xxxx 0xxx xxxx ... 0yyy yyyy

class MidiEvent
{
public:
    MidiEventType type;
    Channel channel;
    Key key;
    Velocity velocity;
    Control control;
    ControlValue controlValue;
    Program program;
    PitchBend pitchBend;
    Timestamp timestamp;

    MidiEvent() = default;
    MidiEvent(std::uint32_t param1, std::uint32_t param2);
    MidiEvent(MidiEventType aType, Channel aChannel, Key aKey, Velocity aVelocity, Timestamp aTimestamp);
    // MidiEvent(MidiEventType aType, Channel aChannel, Control aControl, ControlValue aValue, Timestamp aTimestamp);

    std::uint32_t MakeParam1() const;
    std::uint32_t MakeParam2() const;
};

std::ostream & operator << (std::ostream & stream, const MidiEvent & event);

class IMidiAPI
{
public:
    virtual ~IMidiAPI() = default;

    virtual bool Initialize() = 0;
    virtual void Uninitialize() = 0;
    virtual std::size_t GetNumInputDevices() const = 0;
    virtual std::size_t GetNumOutputDevices() const = 0;
    virtual bool GetCapabilitiesForInputDevice(std::size_t index, MidiCapabilities & capabilities) const = 0;
    virtual bool GetCapabilitiesForOutputDevice(std::size_t index, MidiCapabilities & capabilities) const = 0;
    virtual bool ConnectDevices(IMidiInDevicePtr deviceIn, IMidiOutDevicePtr deviceOut) = 0;
    virtual bool DisconnectDevices(IMidiInDevicePtr deviceIn, IMidiOutDevicePtr deviceOut) = 0;
    virtual IMidiInDevicePtr OpenInputDevice(std::size_t index) = 0;
    virtual IMidiInDevicePtr OpenInputDevice(const std::string & name) = 0;
    virtual IMidiOutDevicePtr OpenOutputDevice(std::size_t index) = 0;
    virtual IMidiOutDevicePtr OpenOutputDevice(const std::string & name) = 0;
    virtual void CloseInputDevice(IMidiInDevicePtr handle) = 0;
    virtual void CloseOutputDevice(IMidiOutDevicePtr handle) = 0;
};

using IMidiAPIPtr = std::unique_ptr<IMidiAPI>;
extern IMidiAPIPtr CreateAPI();

} // namespace midi
