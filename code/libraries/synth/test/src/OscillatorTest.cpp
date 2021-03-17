#include "GoogleTest.h"

#include "synth/Oscillator.h"

#include "utility/Serialization.h"

namespace synth
{
    
static void ShowDiff(const AudioBuffer & expected, const AudioBuffer & actual)
{
    for (std::uint32_t index = 0; index < expected.size(); ++index)
    {
        if (expected[index] != actual[index])
        {
            std::cout << "index=" << index << " expected=" << expected[index] << " actual=" << actual[index] << std::endl;
        }
    }
}

static float CalcDiffRMS(const AudioBuffer & expected, const AudioBuffer & actual)
{
    float sumSquares = 0.0F;
    for (std::uint32_t index = 0; index < expected.size(); ++index)
    {
        sumSquares += ((expected[index] - actual[index]) * (expected[index] - actual[index]));
    }
    return sumSquares / expected.size();
}

TEST(OscillatorTest, ConstructDefault)
{
    Oscillator osc;

    EXPECT_EQ(0.0F, osc.GetFrequency());
    EXPECT_EQ(Waveform::Sine, osc.GetWaveform());
}

TEST(OscillatorTest, SetFrequency)
{
    Oscillator osc;
    float freq = 1.0F;
    osc.SetFrequency(freq);

    EXPECT_EQ(freq, osc.GetFrequency());
    EXPECT_EQ(Waveform::Sine, osc.GetWaveform());
}

TEST(OscillatorTest, SetWaveform)
{
    Oscillator osc;
    Waveform waveform = Waveform::Rectangle;
    osc.SetWaveform(waveform);

    EXPECT_EQ(0.0F, osc.GetFrequency());
    EXPECT_EQ(waveform, osc.GetWaveform());
}

TEST(OscillatorTest, SerializeWaveform)
{
    EXPECT_EQ("Sine", serialization::Serialize(Waveform::Sine));
    EXPECT_EQ("Rectangle", serialization::Serialize(Waveform::Rectangle));
}

TEST(OscillatorTest, SerializeOscillator)
{
    Oscillator osc;
    float freq = 1.0F;
    osc.SetFrequency(freq);
    Waveform waveform = Waveform::Rectangle;
    osc.SetWaveform(waveform);

    EXPECT_EQ("synth::Oscillator: fs=0, bufferSize=0, freq=1, waveform=Rectangle", serialization::Serialize(osc, 0));
}

TEST(OscillatorTest, Prepare)
{
    Oscillator osc;
    std::uint32_t sampleFrequency = 48000;
    std::uint32_t bufferSize = 600;
    osc.Prepare(sampleFrequency, bufferSize);
    EXPECT_EQ(sampleFrequency, osc.GetSampleFrequency());
    EXPECT_EQ(bufferSize, osc.GetBufferSize());
    float freq = 1.0F;
    osc.SetFrequency(freq);
    Waveform waveform = Waveform::Rectangle;
    osc.SetWaveform(waveform);

    EXPECT_EQ("synth::Oscillator: fs=48000, bufferSize=600, freq=1, waveform=Rectangle", serialization::Serialize(osc, 0));
}

TEST(OscillatorTest, GetSamplesSine)
{
    Oscillator osc;
    std::uint32_t sampleFrequency = 48000;
    std::uint32_t bufferSize = 600;
    osc.Prepare(sampleFrequency, bufferSize);
    float freq = 80.0F;
    osc.SetFrequency(freq);
    Waveform waveform = Waveform::Sine;
    osc.SetWaveform(waveform);
    AudioBuffer expected(bufferSize);
    AudioBuffer actual(bufferSize);

    for (std::uint32_t index = 0; index < bufferSize; ++index)
    {
        expected[index] = sin(TwoPi * index * freq / sampleFrequency);
    }
    osc.GetSamples(actual);

    float rms = CalcDiffRMS(expected, actual);
    EXPECT_NEAR(0, rms, 0.01F);
}

TEST(OscillatorTest, GetSamplesRectangle)
{
    Oscillator osc;
    std::uint32_t sampleFrequency = 48000;
    std::uint32_t bufferSize = 600;
    osc.Prepare(sampleFrequency, bufferSize);
    float freq = 80.0F;
    osc.SetFrequency(freq);
    Waveform waveform = Waveform::Rectangle;
    osc.SetWaveform(waveform);
    AudioBuffer expected(bufferSize);
    AudioBuffer actual(bufferSize);

    for (std::uint32_t index = 0; index < bufferSize; ++index)
    {
        expected[index] = (index < bufferSize / 2) ? -1.0F : 1.0F;
    }
    osc.GetSamples(actual);

    float rms = CalcDiffRMS(expected, actual);
    EXPECT_NEAR(0, rms, 0.01F);
}

} // namespace synth
