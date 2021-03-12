#pragma once

#include <string>
#include <vector>
#include "tracing/ConsoleLineWriter.h"

class Application
{
private:
    std::string m_applicationName;
    std::vector<std::string> m_commandLineArguments;
    tracing::ConsoleLineWriter m_writer;

public:
    Application(int argc, char *argv[]);
    virtual ~Application();

    virtual bool CheckArgs();
    virtual void Usage();
    virtual int Run();
};
