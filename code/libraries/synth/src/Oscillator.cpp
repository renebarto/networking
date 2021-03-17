#include "synth/Oscillator.h"

#include <random>
#include "osal/TypeInfo.h"
#include "tracing/Tracing.h"
#include "utility/EnumSerialization.h"
#include "utility/Format.h"

namespace serialization {

template<>
const utility::BidirectionalMap<synth::Waveform, std::string> EnumSerializationMap<synth::Waveform>::ConversionMap = 
{
    {synth::Waveform::Sine, "Sine"},
    {synth::Waveform::Rectangle, "Rectangle"},
    {synth::Waveform::Noise, "Noise"},
};

} // namespace serialization

namespace synth {

const float TwoPi = 8.0F * atan(1.0F);

static unsigned s_seed(static_cast<unsigned>(std::chrono::system_clock::now().time_since_epoch().count()));
static std::default_random_engine s_generator(s_seed);
static std::uniform_real_distribution<float> s_distribution(-1.0F, 1.0F);

std::ostream & operator << (std::ostream & stream, const Waveform & waveform)
{
    return stream << serialization::Serialize(waveform);
}

Oscillator::Oscillator()
    : m_sampleFrequency()
    , m_bufferSize()
    , m_frequency()
    , m_waveform()
    , m_phase()
    , m_phaseStep()
{

}

void Oscillator::SetFrequency(float frequency)
{
    m_frequency = frequency;
    if (m_sampleFrequency != 0)
        m_phaseStep = m_frequency / m_sampleFrequency;
}

float Oscillator::GetFrequency() const
{
    return m_frequency;
}

void Oscillator::SetWaveform(Waveform waveform)
{
    m_waveform = waveform;
}

Waveform Oscillator::GetWaveform() const
{
    return m_waveform;
}

bool Oscillator::Prepare(std::uint32_t samplesPerSecond, std::uint32_t bufferSize)
{
    m_sampleFrequency = samplesPerSecond;
    m_bufferSize = bufferSize;
    return true;
}

std::uint32_t Oscillator::GetSampleFrequency() const
{
    return m_sampleFrequency;
}

std::uint32_t Oscillator::GetBufferSize() const
{
    return m_bufferSize;
}

void Oscillator::GetSamples(AudioBuffer & buffer)
{
    for (std::uint32_t index = 0; index < m_bufferSize; ++index)
    {
        switch (m_waveform)
        {
            case Waveform::Rectangle:
                buffer[index] = (m_phase < 0.5) ? -1.0F : 1.0F;
                break;

            case Waveform::Sine:
                buffer[index] = sin(m_phase * TwoPi);
                break;

            case Waveform::Noise:
                buffer[index] = s_distribution(s_generator);
                break;

            default:
                buffer[index] = 0.0F;
                break;
        }
        m_phase += m_phaseStep;
        if (m_phase >= 1.0F)
            m_phase -= 1.0F;
    }
}

std::ostream & operator << (std::ostream & stream, const Oscillator & oscillator)
{
    return stream << utility::FormatString("{}: fs={}, bufferSize={}, freq={}, waveform={}", 
        osal::type(oscillator), oscillator.GetSampleFrequency(), oscillator.GetBufferSize(),
        oscillator.GetFrequency(), oscillator.GetWaveform());
}

} // namespace synth