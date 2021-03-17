#pragma once

#include <vector>
#include "synth/AudioBuffer.h"

namespace synth {

class IAudioSource
{
public:
    virtual ~IAudioSource() = default;

    virtual bool Prepare(std::uint32_t samplesPerSecond, std::uint32_t bufferSize) = 0;

    virtual std::uint32_t GetSampleFrequency() const = 0;
    virtual std::uint32_t GetBufferSize() const = 0;

    // Get samples (bufferSize)
    virtual void GetSamples(AudioBuffer & buffer) = 0;
};

} // namespace synth