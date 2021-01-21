#pragma once

#include <string>
#include <vector>
#include "osal/Signal.h"

class Application
{
private:
    std::string m_applicationName;
    std::vector<std::string> m_commandLineArguments;
    bool m_interrupted;

public:
    Application(int argc, char *argv[]);
    virtual ~Application();

    virtual bool CheckArgs();
    virtual void Usage();
    virtual int Run();
    void SignalHandler(osal::SignalType signal);
};
