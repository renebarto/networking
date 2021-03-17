#pragma once

#include <iostream>
#include "synth/IAudioSource.h"

namespace synth {

enum class Waveform {
    Sine,
    Rectangle,
    Noise,
};

extern const float TwoPi;

std::ostream & operator << (std::ostream & stream, const Waveform & waveform);

class Oscillator
    : public IAudioSource
{
private:
    std::uint32_t m_sampleFrequency;
    std::uint32_t m_bufferSize;
    float m_frequency;
    Waveform m_waveform;
    float m_phase;
    float m_phaseStep;

public:
    Oscillator();

    void SetFrequency(float frequency);
    float GetFrequency() const;
    void SetWaveform(Waveform waveform);
    Waveform GetWaveform() const;

    bool Prepare(std::uint32_t samplesPerSecond, std::uint32_t bufferSize) override;
    std::uint32_t GetSampleFrequency() const override;
    std::uint32_t GetBufferSize() const override;

    // Get samples (bufferSize)
    void GetSamples(AudioBuffer & buffer) override;
};

std::ostream & operator << (std::ostream & stream, const Oscillator & oscillator);

} // namespace synth