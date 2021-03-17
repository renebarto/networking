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
#include "synth/IMultiAudioSource.h"

namespace synth {

struct SynthDefinition
{
    std::string type;
    std::string name;
};

class ISynth
    : public IMultiAudioSource
{
public:
    virtual ~ISynth() = default;

    virtual bool Initialize(const std::string & configuration) = 0;
    virtual void Uninitialize() = 0;
    virtual bool IsInitialized() = 0;

    virtual bool Prepare(std::uint32_t samplesPerSecond, std::uint16_t numChannels, std::uint32_t bufferSize) = 0;
    virtual void GetSamples(MultiAudioBuffer & buffer) = 0;
};

using ISynthPtr = std::shared_ptr<ISynth>;

} // namespace synth
