//------------------------------------------------------------------------------
// Copyright   : Copyright(c) 2020, Rene Barto
//
// File        : MidiDevice.h
//
// Namespace   : midi
//
// Class       : -
//
// Description : 
//
//------------------------------------------------------------------------------

#pragma once

#include <functional>
#include <string>
#include "IMidiAPI.h"

namespace midi {

class IMidiAPI;

class MidiOutDeviceImpl;

class IMidiOutDevice
{
public:
    virtual ~IMidiOutDevice() = default;

    virtual bool Open(std::size_t index) = 0;
    virtual bool IsOpen() const = 0;
    virtual void Close() = 0;

    virtual void SendMidiOutEvent(std::uint32_t msg) = 0;

    virtual void SendMidiOutEvent(const MidiEvent & event) = 0;
};

extern IMidiOutDevicePtr CreateMidiOutDevice(IMidiAPI * api);

} // namespace midi
