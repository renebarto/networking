#pragma once

#include "synth/IAudioSource.h"
#include "synth/MultiAudioBuffer.h"

namespace synth {

class IMultiAudioSource
{
public:
    virtual ~IMultiAudioSource() = default;

    virtual bool Prepare(std::uint32_t samplesPerSecond, std::uint16_t numChannels, std::uint32_t bufferSize) = 0;

    virtual std::uint32_t GetSampleFrequency() const = 0;
    virtual std::uint32_t GetBufferSize() const = 0;
    virtual std::uint32_t GetNumChannels() const = 0;

    // Get samples (bufferSize)
    virtual void GetSamples(MultiAudioBuffer & buffer) = 0;
};

} // namespace synth