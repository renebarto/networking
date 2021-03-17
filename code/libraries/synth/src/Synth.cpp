//------------------------------------------------------------------------------
// Copyright   : Copyright(c) 2020, Rene Barto
//
// File        : Synth.cpp
//
// Namespace   : synth
//
// Class       : -
//
// Description : 
//
//------------------------------------------------------------------------------

#include "tracing/ScopedTracing.h"
#include "utility/EnumSerialization.h"
#include "midi/IMidiAPI.h"
#include "synth/Synth.h"

namespace synth {

Synth::Synth()
    : m_isInitialized()
    , m_sampleFrequency()
    , m_numChannels()
    , m_bufferSize()
{
    SCOPEDTRACE(
        nullptr, 
        nullptr);
}

Synth::~Synth()
{
    SCOPEDTRACE(
        nullptr, 
        nullptr);
    Uninitialize();
}

bool Synth::Initialize(const std::string & configuration)
{
    bool result {};
    SCOPEDTRACE(
        [&] () { return utility::FormatString("configuration={}", configuration); }, 
        [&] () { return utility::FormatString("result={}", result); });
    if (m_isInitialized)
    {
        result = true;
        return result;
    }

    m_isInitialized = true;
    result = true;
    return result;
}

void Synth::Uninitialize()
{
    SCOPEDTRACE(
        nullptr, 
        nullptr);
    m_isInitialized = false;
}

bool Synth::IsInitialized()
{
    return m_isInitialized;
}

bool Synth::Prepare(std::uint32_t samplesPerSecond, std::uint16_t numChannels, std::uint32_t bufferSize)
{
    bool result {};
    SCOPEDTRACE(
        [&] () { return utility::FormatString("samplesPerSecond={}, numChannels={}, bufferSize={}", samplesPerSecond, numChannels, bufferSize); }, 
        [&] () { return utility::FormatString("result={}", result); });
    m_sampleFrequency = samplesPerSecond;
    m_numChannels = numChannels;
    m_bufferSize = bufferSize;

    if (!m_isInitialized)
    {
        LogError(__FILE__, __LINE__, __func__, "Not initialized");
        return result;
    }

    result = true;
    return result;
}

void Synth::GetSamples(MultiAudioBuffer & /*buffer*/)
{
    SCOPEDTRACE(
        nullptr, 
        nullptr);
}

std::uint32_t Synth::GetSampleFrequency() const
{
    return m_sampleFrequency;
}

std::uint32_t Synth::GetBufferSize() const
{
    return m_bufferSize;
}

std::uint32_t Synth::GetNumChannels() const
{
    return m_numChannels;
}

} // namespace synth
