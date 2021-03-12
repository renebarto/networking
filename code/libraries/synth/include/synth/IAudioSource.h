#pragma once

#include <vector>

namespace synth {

class IAudioSource
{
public:
    virtual ~IAudioSource() = default;

    virtual void Prepare(std::uint32_t samplesPerSecond, std::uint32_t bufferSize) = 0;

    // Get samples (bufferSize)
    virtual void GetSamples(std::vector<float> & buffer) = 0;
};

} // namespace synth