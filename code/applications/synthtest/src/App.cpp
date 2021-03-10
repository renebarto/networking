#include "App.h"

#include <functional>
#include <conio.h>
#include "osal/Thread.h"
#include "tracing/Logging.h"
#include "tracing/Tracing.h"
#include "utility/GenericError.h"

Application::Application(int argc, char *argv[])
    : m_applicationName(argv[0])
    , m_commandLineArguments()
    , m_interrupted()
    , m_api()
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
    tracing::SetDefaultTraceFilter(tracing::TraceCategory::Message | tracing::TraceCategory::Data/* | tracing::TraceCategory::FunctionEnter | tracing::TraceCategory::FunctionLeave*/);
    osal::SetThreadNameSelf("Main");
    osal::SetSignalHandler(osal::SignalType::Interrupt, std::bind(&Application::SignalHandler, this, std::placeholders::_1));

    m_api = sound::CreateAPI();
    if (!m_api->Initialize("Digital Audio (S/PDIF) (High Definition Audio Device)"))
    {
        tracing::Logging::Fatal(__FILE__, __LINE__, __func__, utility::GenericError("Cannot initialize Sound API"));
    }

    m_api->Start(&m_synth);

    bool quit = false;
    while (!quit)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        if (_getch() == 'q')
            quit = true;
    }

    m_api->Stop();

    m_api->Uninitialize();
    m_api.reset();
    
    return 0;
}

void Application::SignalHandler(osal::SignalType signal)
{
    TraceMessage(__FILE__, __LINE__, __func__, "Caught signal {}", signal);
    if (signal == osal::SignalType::Interrupt)
        m_interrupted = true;
}
