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

#include "synth/ISynth.h"

namespace synth {

class Synth
    : public ISynth
{
private:
    bool m_isInitialized;
    std::uint32_t m_sampleFrequency;
    std::uint16_t m_numChannels;
    std::uint32_t m_bufferSize;

public:
    Synth();
    virtual ~Synth();

    bool Initialize(const std::string & configuration) override;
    void Uninitialize() override;
    bool IsInitialized() override;

    std::uint32_t GetSampleFrequency() const override;
    std::uint32_t GetBufferSize() const override;
    std::uint32_t GetNumChannels() const override;

    bool Prepare(std::uint32_t samplesPerSecond, std::uint16_t numChannels, std::uint32_t bufferSize) override;
    void GetSamples(MultiAudioBuffer & buffer) override;
};

} // namespace synth
