#include "GoogleTest.h"

#include "synth/Oscillator.h"

#include "utility/Serialization.h"

namespace synth
{
    
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

    EXPECT_EQ("synth::Oscillator: freq: 1, waveform: Rectangle", serialization::Serialize(osc, 0));
}

} // namespace synth
