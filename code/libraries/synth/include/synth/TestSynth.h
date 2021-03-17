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

#include "synth/Synth.h"

namespace synth {

class Oscillator;

class TestSynth
    : public Synth
{
private:
    std::shared_ptr<Oscillator> m_oscLeft;
    std::shared_ptr<Oscillator> m_oscRight;
    AudioBuffer m_bufferLeft;
    AudioBuffer m_bufferRight;

public:
    TestSynth();
    virtual ~TestSynth();

    bool Initialize(const std::string & configuration) override;

    bool Prepare(std::uint32_t samplesPerSecond, std::uint16_t numChannels, std::uint32_t bufferSize) override;
    void GetSamples(MultiAudioBuffer & buffer) override;
};

} // namespace synth
