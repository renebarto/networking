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

#include "synth/Synth.h"

namespace synth {

Synth::Synth()
    : m_isInitialized()
    , m_samplesPerSecond()
    , m_numChannels()
    , m_bufferSize()
    , m_phase()
    , m_phaseStep()
    , m_seed(static_cast<unsigned>(std::chrono::system_clock::now().time_since_epoch().count()))
    , m_generator(m_seed)
    , m_distribution(-1.0F, 1.0F)
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

bool Synth::Initialize()
{
    bool result {};
    SCOPEDTRACE(
        nullptr, 
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

static const float TwoPi = 8 * atan(1.0F);

void Synth::Prepare(std::uint32_t samplesPerSecond, std::uint16_t numChannels, std::uint32_t bufferSize)
{
    m_samplesPerSecond = samplesPerSecond;
    m_numChannels = numChannels;
    m_bufferSize = bufferSize;
    m_phase = 0;
    m_phaseStep = TwoPi * 1000.0F / m_samplesPerSecond;
}

void Synth::GetSamples(std::vector<std::vector<float>> & buffer)
{
    for (std::size_t frameIndex = 0; frameIndex < m_bufferSize; ++frameIndex)
    {
        float currentSample = sin(m_phase);
        for (std::uint16_t channel = 0; channel < m_numChannels; ++channel)
        {
            if (channel == 0)
                buffer[channel][frameIndex] = currentSample;
            else
                buffer[channel][frameIndex] = m_distribution(m_generator);
        }
        m_phase += m_phaseStep;
        if (m_phase > TwoPi)
            m_phase -= TwoPi;
    }
}

} // namespace synth
