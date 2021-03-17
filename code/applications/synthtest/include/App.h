#pragma once

#include <random>
#include <string>
#include <vector>
#include "osal/Signal.h"
#include "midi/IMidiAPI.h"
#include "sound/ISoundAPI.h"
#include "synth/ISynthRack.h"

class Application
{
private:
    std::string m_applicationName;
    std::vector<std::string> m_commandLineArguments;
    bool m_interrupted;
    sound::ISoundAPIPtr m_soundAPI;
    synth::ISynthRackPtr m_synthRack;
    midi::IMidiAPIPtr m_midiAPI;
    midi::IMidiInDevicePtr m_midiDeviceIn;

public:
    Application(int argc, char *argv[]);
    virtual ~Application();

    virtual bool CheckArgs();
    virtual void Usage();
    virtual int Run();
    void SignalHandler(osal::SignalType signal);
};
