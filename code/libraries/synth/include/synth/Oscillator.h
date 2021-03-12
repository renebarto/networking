#pragma once

#include <iostream>
#include "synth/IAudioSource.h"

namespace synth {

enum class Waveform {
    Sine,
    Rectangle,
};

std::ostream & operator << (std::ostream & stream, const Waveform & waveform);

class Oscillator
    : public IAudioSource
{
private:
    float m_frequency;
    Waveform m_waveform;

public:
    Oscillator();

    void SetFrequency(float frequency);
    float GetFrequency() const;
    void SetWaveform(Waveform waveform);
    Waveform GetWaveform() const;

    void Prepare(std::uint32_t samplesPerSecond, std::uint32_t bufferSize) override;

    // Get samples (bufferSize)
    void GetSamples(std::vector<float> & buffer) override;
};

std::ostream & operator << (std::ostream & stream, const Oscillator & oscillator);

} // namespace synth