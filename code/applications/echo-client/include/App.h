#pragma once

#include <string>
#include <vector>

class Application
{
private:
    std::string m_applicationName;
    std::vector<std::string> m_commandLineArguments;

public:
    Application(int argc, char *argv[]);
    virtual ~Application();

    virtual bool CheckArgs();
    virtual void Usage();
    virtual int Run();
};
