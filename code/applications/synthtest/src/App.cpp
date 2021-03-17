#include "App.h"

#include <functional>
#include <conio.h>
#include "osal/ThreadFunctions.h"
#include "tracing/Tracing.h"
#include "utility/GenericError.h"
#include "midi/IMidiInDevice.h"
#include "synth/SynthRack.h"

Application::Application(int argc, char *argv[])
    : m_applicationName(argv[0])
    , m_commandLineArguments()
    , m_interrupted()
    , m_soundAPI()
    , m_synthRack()
    , m_midiAPI()
    , m_midiDeviceIn()
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
    osal::SetSignalHandler(osal::SignalType::Interrupt, std::bind(&Application::SignalHandler, this, std::placeholders::_1));

    m_soundAPI = sound::CreateAPI();
    if (!m_soundAPI->Initialize("Digital Audio (S/PDIF) (High Definition Audio Device)"))
    {
        tracing::Tracing::Fatal(__FILE__, __LINE__, __func__, utility::GenericError("Cannot initialize Sound API"));
    }

    m_midiAPI = midi::CreateAPI();
    if (!m_midiAPI->Initialize())
    {
        tracing::Tracing::Fatal(__FILE__, __LINE__, __func__, utility::GenericError("Cannot initialize MIDI API"));
    }

    m_synthRack = std::make_shared<synth::SynthRack>();
    if (!m_synthRack->Initialize(
        "[\n"
            "{\n"
                "\"type\": \"TestSynth\",\n"
                "\"name\": \"TestSynth\"\n"
            "}\n"
        "]"))
    {
        tracing::Tracing::Fatal(__FILE__, __LINE__, __func__, utility::GenericError("Cannot initialize Synth Rack"));
    }

    m_midiDeviceIn = m_midiAPI->OpenInputDevice("ESI KeyControl 25 XT");
    m_midiDeviceIn->SetEventCallback(std::bind(&synth::ISynthRack::OnMidiEvent, m_synthRack.get(), std::placeholders::_1));
    m_soundAPI->Start(m_synthRack.get());
    m_midiDeviceIn->Start();

    bool quit = false;
    while (!quit)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        if (_getch() == 'q')
            quit = true;
    }

    m_soundAPI->Stop();
    m_midiDeviceIn->Stop();
    m_midiDeviceIn.reset();

    m_soundAPI->Uninitialize();
    m_soundAPI.reset();
    m_midiAPI->Uninitialize();
    m_midiAPI.reset();
    
    return 0;
}

void Application::SignalHandler(osal::SignalType signal)
{
    TraceInfo(__FILE__, __LINE__, __func__, "Caught signal {}", signal);
    if (signal == osal::SignalType::Interrupt)
        m_interrupted = true;
}
