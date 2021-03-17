//------------------------------------------------------------------------------
// Copyright   : Copyright(c) 2020, Rene Barto
//
// File        : Synth.h
//
// Namespace   : synth
//
// Class       : -
//
// Description : 
//
//------------------------------------------------------------------------------

#pragma once

#include <memory>
#include "midi/IMidiAPI.h"
#include "sound/ISoundSource.h"

namespace synth {

class ISynth;
using ISynthPtr = std::shared_ptr<ISynth>;

class ISynthRack
    : public sound::ISoundSource
{
public:
    virtual ~ISynthRack() = default;

    virtual bool Initialize(const std::string & configuration) = 0;
    virtual void Uninitialize() = 0;
    virtual bool IsInitialized() = 0;

    virtual void AddSynth(ISynthPtr synth) = 0;
    virtual void RemoveSynth(ISynthPtr synth) = 0;

    virtual void OnMidiEvent(const midi::MidiEvent & event) = 0;
};

using ISynthRackPtr = std::shared_ptr<ISynthRack>;

} // namespace synth
