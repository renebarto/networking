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

#include <random>
#include "sound/ISoundSource.h"

namespace midi {

class MidiEvent;

} // namespace midi

namespace synth {

class Synth
    : public sound::ISoundSource
{
private:
    bool m_isInitialized;
    std::uint32_t m_samplesPerSecond;
    std::uint16_t m_numChannels;
    std::uint32_t m_bufferSize;
    float m_phase;
    float m_phaseStep;
    unsigned m_seed;
    std::default_random_engine m_generator;
    std::uniform_real_distribution<float> m_distribution;

public:
    Synth();
    virtual ~Synth();

    bool Initialize();
    void Uninitialize();
    bool IsInitialized();

    void Prepare(std::uint32_t samplesPerSecond, std::uint16_t numChannels, std::uint32_t bufferSize) override;
    void GetSamples(std::vector<std::vector<float>> & buffer) override;

    void OnMidiEvent(const midi::MidiEvent & event);
};

} // namespace synth
