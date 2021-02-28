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

using MidiEventCallback = std::function<void (const MidiEvent &)>;

class IMidiInDevice
{
public:
    virtual ~IMidiInDevice() = default;

    virtual bool Open(std::size_t index) = 0;
    virtual bool IsOpen() const = 0;
    virtual void Close() = 0;

    virtual bool Start() = 0;
    virtual bool Stop() = 0;
    virtual bool IsStarted() = 0;

    virtual void OnMidiEvent(std::uint32_t msg, std::uint32_t param1, std::uint32_t param2) = 0;

    virtual void SetEventCallback(MidiEventCallback callback) = 0;
};

extern IMidiInDevicePtr CreateMidiInDevice(IMidiAPI * api);

} // namespace midi
