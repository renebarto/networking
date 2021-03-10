#pragma once

#include <random>
#include <string>
#include <vector>
#include "osal/Signal.h"
#include "sound/ISoundAPI.h"
#include "synth/Synth.h"

class Application
{
private:
    std::string m_applicationName;
    std::vector<std::string> m_commandLineArguments;
    bool m_interrupted;
    sound::ISoundAPIPtr m_api;
    synth::Synth m_synth;

public:
    Application(int argc, char *argv[]);
    virtual ~Application();

    virtual bool CheckArgs();
    virtual void Usage();
    virtual int Run();
    void SignalHandler(osal::SignalType signal);
};
