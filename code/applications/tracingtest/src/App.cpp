#include "App.h"

#include <functional>
#include "osal/ThreadFunctions.h"
#include "tracing/Tracing.h"
#include "tracing/TraceWriter.h"
#include "utility/GenericError.h"

Application::Application(int argc, char *argv[])
    : m_applicationName(argv[0])
    , m_commandLineArguments()
    , m_writer()
{
    for (int i = 1; i < argc; ++i)
    {
        m_commandLineArguments.push_back(argv[i]);
    }
}

Application::~Application()
{
}

bool Application::CheckArgs()
{
    std::cout << m_applicationName << "main(" << m_commandLineArguments.size() << ", [";
    bool firstArgument = true;
    for (auto const & argument: m_commandLineArguments)
    {
        if (!firstArgument)
            std::cout << ", ";
        std::cout << argument;
        firstArgument = false;
    }
    std::cout << "])" << std::endl;
    // if (argc < 2)
    // {
    //     return false;
    // }
    return true;
}

void Application::Usage()
{
    std::cerr << "Usage: " << m_applicationName << " <server IP>" << std::endl;
}

int Application::Run()
{
    tracing::SetDefaultTraceFilter(tracing::TraceCategory::Information | tracing::TraceCategory::Data | tracing::TraceCategory::FunctionEnter | tracing::TraceCategory::FunctionLeave);
    osal::SetThreadNameSelf("Main");
    tracing::TraceWriter traceWriter(m_writer);
    tracing::Tracing::SetTracingFunction(
        std::bind(&tracing::TraceWriter::Trace, &traceWriter, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, 
                  std::placeholders::_4, std::placeholders::_5, std::placeholders::_6));

    TraceInfo(__FILE__, __LINE__, __func__, "Starting");


    TraceInfo(__FILE__, __LINE__, __func__, "Stopping");

    return 0;
}

