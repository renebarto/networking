//------------------------------------------------------------------------------
// Copyright   : Copyright(c) 2020, Rene Barto
//
// File        : ISoundAPI.h
//
// Namespace   : midi
//
// Class       : -
//
// Description : 
//
//------------------------------------------------------------------------------

#pragma once

#include <vector>

namespace sound {

enum RenderSampleType
{
    SampleTypeFloat,
    SampleType16BitPCM,
};

using SoundBuffer = std::vector<std::vector<float>>;

class ISoundSource
{
public:
    virtual ~ISoundSource() = default;

    virtual bool Prepare(std::uint32_t samplesPerSecond, std::uint16_t numChannels, std::uint32_t bufferSize) = 0;

    // Get samples (numChannels, bufferSize)
    virtual void GetSamples(SoundBuffer & buffer) = 0;
};

} // namespace sound
