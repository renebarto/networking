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

class IAudioSource
{
public:
    virtual ~IAudioSource() = default;

    virtual void Prepare(std::uint32_t samplesPerSecond, std::uint16_t numChannels, std::uint32_t bufferSize) = 0;

    // Get samples (numChannels, bufferSize)
    virtual void GetSamples(std::vector<std::vector<float>> & buffer) = 0;
};

} // namespace sound
