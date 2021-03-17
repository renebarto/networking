//------------------------------------------------------------------------------
// Copyright   : Copyright(c) 2020, Rene Barto
//
// File        : TestSynth.cpp
//
// Namespace   : synth
//
// Class       : -
//
// Description : 
//
//------------------------------------------------------------------------------

#include <algorithm>
#include "tracing/ScopedTracing.h"
#include "utility/EnumSerialization.h"
#include "synth/TestSynth.h"
#include "synth/Oscillator.h"

namespace synth {

TestSynth::TestSynth()
    : m_oscLeft()
    , m_oscRight()
    , m_bufferLeft()
    , m_bufferRight()
{
    SCOPEDTRACE(
        nullptr, 
        nullptr);
}

TestSynth::~TestSynth()
{
    SCOPEDTRACE(
        nullptr, 
        nullptr);
    Uninitialize();
}

bool TestSynth::Initialize(const std::string & configuration)
{
    bool result {};
    SCOPEDTRACE(
        [&] () { return utility::FormatString("configuration={}", configuration); }, 
        [&] () { return utility::FormatString("result={}", result); });

    if (IsInitialized())
    {
        result = true;
        return result;
    }

    if (!Synth::Initialize(configuration))
    {
        LogError(__FILE__, __LINE__, __func__, "Initialize failed");
        return result;
    }

    m_oscLeft = std::make_shared<Oscillator>();
    m_oscRight = std::make_shared<Oscillator>();

    result = true;
    return result;
}

bool TestSynth::Prepare(std::uint32_t samplesPerSecond, std::uint16_t numChannels, std::uint32_t bufferSize)
{
    bool result {};
    SCOPEDTRACE(
        [&] () { return utility::FormatString("samplesPerSecond={}, numChannels={}, bufferSize={}", samplesPerSecond, numChannels, bufferSize); }, 
        [&] () { return utility::FormatString("result={}", result); });

    if (!Synth::Prepare(samplesPerSecond, numChannels, bufferSize))
    {
        LogError(__FILE__, __LINE__, __func__, "Prepare failed");
        return result;
    }
    
    if (!m_oscLeft->Prepare(GetSampleFrequency(), GetBufferSize()))
        return result;
    if (!m_oscRight->Prepare(GetSampleFrequency(), GetBufferSize()))
        return result;
    m_oscLeft->SetFrequency(1000.0F);
    m_oscLeft->SetWaveform(Waveform::Sine);
    m_oscRight->SetFrequency(500.0F);
    m_oscRight->SetWaveform(Waveform::Sine);

    m_bufferLeft.resize(GetBufferSize());
    m_bufferRight.resize(GetBufferSize());

    result = true;
    return result;
}

void TestSynth::GetSamples(MultiAudioBuffer & buffer)
{
    SCOPEDTRACE(
        nullptr, 
        nullptr);

    m_oscLeft->GetSamples(m_bufferLeft);
    m_oscRight->GetSamples(m_bufferRight);
    for (auto & channelBuffer : buffer)
    {
        std::fill(channelBuffer.begin(), channelBuffer.end(), 0.0F);
    }
    std::transform(buffer[0].begin(), buffer[0].end(), m_bufferLeft.begin(), buffer[0].begin(), std::plus<float>());
    std::transform(buffer[1].begin(), buffer[1].end(), m_bufferRight.begin(), buffer[1].begin(), std::plus<float>());
}

} // namespace synth
