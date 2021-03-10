#pragma once

#include <random>
#include <string>
#include <vector>
#include "osal/Signal.h"
#include "sound/ISoundAPI.h"
#include "sound/IAudioSource.h"

class Application
    : public sound::IAudioSource
{
private:
    std::string m_applicationName;
    std::vector<std::string> m_commandLineArguments;
    bool m_interrupted;
    sound::ISoundAPIPtr m_api;
    std::uint32_t m_samplesPerSecond;
    std::uint16_t m_numChannels;
    std::uint32_t m_bufferSize;
    float m_phase;
    float m_phaseStep;
    unsigned m_seed;
    std::default_random_engine m_generator;
    std::uniform_real_distribution<float> m_distribution;

public:
    Application(int argc, char *argv[]);
    virtual ~Application();

    virtual bool CheckArgs();
    virtual void Usage();
    virtual int Run();
    void SignalHandler(osal::SignalType signal);

    void Prepare(std::uint32_t samplesPerSecond, std::uint16_t numChannels, std::uint32_t bufferSize) override;
    void GetSamples(std::vector<std::vector<float>> & buffer) override;
};
