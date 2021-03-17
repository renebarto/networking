#include "App.h"

#include <functional>
#include <conio.h>
#include "osal/ThreadFunctions.h"
#include "tracing/Logging.h"
#include "tracing/Tracing.h"
#include "utility/GenericError.h"

Application::Application(int argc, char *argv[])
    : m_applicationName(argv[0])
    , m_commandLineArguments()
    , m_interrupted()
    , m_api()
    , m_samplesPerSecond()
    , m_numChannels()
    , m_bufferSize()
    , m_phase()
    , m_phaseStep()
    , m_seed(static_cast<unsigned>(std::chrono::system_clock::now().time_since_epoch().count()))
    , m_generator(m_seed)
    , m_distribution(-1.0F, 1.0F)
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

    m_api->Start(this);

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

static const float TwoPi = 8 * atan(1.0F);

bool Application::Prepare(std::uint32_t samplesPerSecond, std::uint16_t numChannels, std::uint32_t bufferSize)
{
    m_samplesPerSecond = samplesPerSecond;
    m_numChannels = numChannels;
    m_bufferSize = bufferSize;
    m_phase = 0;
    m_phaseStep = TwoPi * 1000.0F / m_samplesPerSecond;
    return true;
}

void Application::GetSamples(std::vector<std::vector<float>> & buffer)
{
    for (std::size_t frameIndex = 0; frameIndex < m_bufferSize; ++frameIndex)
    {
        float currentSample = sin(m_phase);
        for (std::uint16_t channel = 0; channel < m_numChannels; ++channel)
        {
            if (channel == 0)
                buffer[channel][frameIndex] = currentSample;
            else
                buffer[channel][frameIndex] = m_distribution(m_generator);
        }
        m_phase += m_phaseStep;
        if (m_phase > TwoPi)
            m_phase -= TwoPi;
    }
}
