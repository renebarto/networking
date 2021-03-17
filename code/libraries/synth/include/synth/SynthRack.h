//------------------------------------------------------------------------------
// Copyright   : Copyright(c) 2020, Rene Barto
//
// File        : SynthRack.h
//
// Namespace   : synth
//
// Class       : -
//
// Description : 
//
//------------------------------------------------------------------------------

#pragma once

#include <mutex>
#include "synth/ISynthRack.h"
#include "synth/Synth.h"

namespace midi {

class MidiEvent;

} // namespace midi

namespace synth {

class SynthRack
    : public synth::ISynthRack
{
private:
    bool m_isInitialized;
    std::uint32_t m_samplesPerSecond;
    std::uint16_t m_numChannels;
    std::uint32_t m_bufferSize;
    using Mutex = std::recursive_mutex;
    using Lock = std::lock_guard<Mutex>;
    Mutex m_synthsGuard;
    std::vector<ISynthPtr> m_synths;
    std::vector<MultiAudioBuffer> m_synthBuffers;

public:
    SynthRack();
    virtual ~SynthRack();

    bool Initialize(const std::string & configuration) override;
    void Uninitialize() override;
    bool IsInitialized() override;

    void AddSynth(ISynthPtr synth) override;
    void RemoveSynth(ISynthPtr synth) override;

    bool Prepare(std::uint32_t samplesPerSecond, std::uint16_t numChannels, std::uint32_t bufferSize) override;
    void GetSamples(sound::SoundBuffer & buffer) override;

    void OnMidiEvent(const midi::MidiEvent & event) override;
};

} // namespace synth
