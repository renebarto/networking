#include "synth/Oscillator.h"

#include "osal/TypeInfo.h"
#include "utility/EnumSerialization.h"
#include "utility/Format.h"

namespace serialization {

template<>
const utility::BidirectionalMap<synth::Waveform, std::string> EnumSerializationMap<synth::Waveform>::ConversionMap = 
{
    {synth::Waveform::Sine, "Sine"},
    {synth::Waveform::Rectangle, "Rectangle"},
};

} // namespace serialization

namespace synth {

std::ostream & operator << (std::ostream & stream, const Waveform & waveform)
{
    return stream << serialization::Serialize(waveform);
}

Oscillator::Oscillator()
    : m_frequency()
    , m_waveform()
{

}

void Oscillator::SetFrequency(float frequency)
{
    m_frequency = frequency;
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

void Oscillator::Prepare(std::uint32_t /*samplesPerSecond*/, std::uint32_t /*bufferSize*/)
{

}

void Oscillator::GetSamples(std::vector<float> & /*buffer*/)
{

}

std::ostream & operator << (std::ostream & stream, const Oscillator & oscillator)
{
    return stream << utility::FormatString("{}: freq: {}, waveform: {}", osal::type(oscillator), oscillator.GetFrequency(), oscillator.GetWaveform());
}

} // namespace synth