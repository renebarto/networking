//------------------------------------------------------------------------------
// Copyright   : Copyright(c) 2020, Rene Barto
//
// File        : SynthRack.cpp
//
// Namespace   : synth
//
// Class       : -
//
// Description : 
//
//------------------------------------------------------------------------------

#include <algorithm>
#include "tracing/Logging.h"
#include "tracing/Tracing.h"
#include "tracing/ScopedTracing.h"
#include "utility/EnumSerialization.h"
#include "midi/IMidiAPI.h"
#include "json/json.h"
#include "synth/SynthRack.h"
#include "synth/TestSynth.h"
#include "synth/SynthFactory.h"

namespace synth {

SynthRack::SynthRack()
    : m_isInitialized()
    , m_samplesPerSecond()
    , m_numChannels()
    , m_bufferSize()
    , m_synthsGuard()
    , m_synths()
    , m_synthBuffers()
{
    SCOPEDTRACE(
        nullptr, 
        nullptr);
}

SynthRack::~SynthRack()
{
    SCOPEDTRACE(
        nullptr, 
        nullptr);
    Uninitialize();
}

std::string GetType(std::shared_ptr<json::Object> object)
{
    std::string result;
    auto it = object->GetIterator();
    while (!it.AtEnd())
    {
        if (utility::IsEqualIgnoreCase("type", (*it).GetKey()))
        {
            auto nameValue = (*it).GetValue();
            if (json::IsString(nameValue))
            {
                result = json::ToString(nameValue)->GetValue();
            }
            else
            {
                LogError(__FILE__, __LINE__, __func__, "Configuration error: name not a string: {}", json::Serialize(*object));
            }
            break;
        }
        it++;
    }
    return result;
}

bool SynthRack::Initialize(const std::string & configuration)
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

    std::istringstream stream(configuration);
    auto object = json::Parse(stream);
    auto synthList = json::ToArray(object);
    if (synthList == nullptr)
    {
        LogError(__FILE__, __LINE__, __func__, "Configuration error: not an array: {}", configuration);
        return result;
    }
    auto it = synthList->GetIterator();
    SynthFactory factory;
    while (!it.AtEnd())
    {
        auto synthItem = json::ToObject(*it);
        if (synthItem == nullptr)
        {
            LogError(__FILE__, __LINE__, __func__, "Configuration error: not an object: {}", (*it));
            return result;
        }
        std::string type = GetType(synthItem);
        ISynthPtr synth = factory.Create(type);
        if (!synth->Initialize(json::Serialize(*synthItem)))
            return result;

        AddSynth(synth);
        it++;
    }

    m_isInitialized = true;
    result = true;
    return result;
}

void SynthRack::Uninitialize()
{
    SCOPEDTRACE(
        nullptr, 
        nullptr);

    Lock lock(m_synthsGuard);
    for (auto & synth : m_synths)
    {
        synth->Uninitialize();
    }
    m_isInitialized = false;
}

bool SynthRack::IsInitialized()
{
    return m_isInitialized;
}

void SynthRack::AddSynth(ISynthPtr synth)
{
    Lock lock(m_synthsGuard);
    m_synths.push_back(synth);
    m_synthBuffers.push_back(MultiAudioBuffer(m_numChannels, m_bufferSize));
}

void SynthRack::RemoveSynth(ISynthPtr synth)
{
    Lock lock(m_synthsGuard);
    long indexFound {};
    bool found {};
    for (std::size_t index = 0; index < m_synths.size(); ++index)
    {
        if (m_synths[index] == synth)
        {
            found = true;
            indexFound = static_cast<long>(index);
            break;
        }
    }
    if (found)
    {
        m_synths.erase(m_synths.begin() + indexFound);
        m_synthBuffers.erase(m_synthBuffers.begin() + indexFound);
    }
}

bool SynthRack::Prepare(std::uint32_t samplesPerSecond, std::uint16_t numChannels, std::uint32_t bufferSize)
{
    bool result {};
    SCOPEDTRACE(
        [&] () { return utility::FormatString("samplesPerSecond={}, numChannels={}, bufferSize={}", samplesPerSecond, numChannels, bufferSize); }, 
        [&] () { return utility::FormatString("result={}", result); });

    m_samplesPerSecond = samplesPerSecond;
    m_numChannels = numChannels;
    m_bufferSize = bufferSize;

    if (!m_isInitialized)
    {
        LogError(__FILE__, __LINE__, __func__, "Not initialized");
        return result;
    }

    Lock lock(m_synthsGuard);
    for (auto & synth : m_synths)
    {
        if (!synth->Prepare(samplesPerSecond, numChannels, bufferSize))
            return result;
    }
    for (auto & buffer : m_synthBuffers)
    {
        buffer.Resize(m_numChannels, m_bufferSize);
    }

    result = true;
    return result;
}

void SynthRack::GetSamples(std::vector<std::vector<float>> & buffer)
{
    Lock lock(m_synthsGuard);
    for (auto & channelBuffer : buffer)
    {
        std::fill(channelBuffer.begin(), channelBuffer.end(), 0.0F);
    }
    for (std::size_t index = 0; index < m_synths.size(); ++index)
    {
        m_synths[index]->GetSamples(m_synthBuffers[index]);
        for (std::uint16_t channel = 0; channel < m_numChannels; ++channel)
        {
            std::transform(buffer[channel].begin(), buffer[channel].end(), m_synthBuffers[index][channel].begin(), buffer[channel].begin(), std::plus<float>());
        }
    }
}

void SynthRack::OnMidiEvent(const midi::MidiEvent & event)
{
    TraceMessage(__FILE__, __LINE__, __func__, "event={}", event);
    switch (event.type)
    {
        case midi::MidiEventType::NoteOn:
        case midi::MidiEventType::NoteOff:
            // m_deviceOut->SendMidiOutEvent(midi::MidiEvent(event.type, event.channel, static_cast<midi::Key>(event.key + 12), event.velocity, event.timestamp));
            // m_deviceOut->SendMidiOutEvent(midi::MidiEvent(event.type, event.channel, static_cast<midi::Key>(event.key + 16), event.velocity, event.timestamp));
            // m_deviceOut->SendMidiOutEvent(midi::MidiEvent(event.type, event.channel, static_cast<midi::Key>(event.key + 21), event.velocity, event.timestamp));
            break;
        case midi::MidiEventType::ControlChange:
            break;
        case midi::MidiEventType::PitchBend:
            break;
        default:
            break;
    }
}

} // namespace synth
