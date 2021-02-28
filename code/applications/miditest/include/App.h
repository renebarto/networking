#pragma once

#include <string>
#include <vector>
#include "osal/Signal.h"
#include "midi/IMidiAPI.h"

class Application
{
private:
    std::string m_applicationName;
    std::vector<std::string> m_commandLineArguments;
    bool m_interrupted;
    midi::IMidiAPIPtr m_api;
    midi::IMidiInDevicePtr m_deviceIn;
    midi::IMidiOutDevicePtr m_deviceOut;

public:
    Application(int argc, char *argv[]);
    virtual ~Application();

    virtual bool CheckArgs();
    virtual void Usage();
    virtual int Run();
    void SignalHandler(osal::SignalType signal);

    void OnMidiEvent(const midi::MidiEvent & event);
};
