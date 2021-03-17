#include "App.h"

#include <functional>
#include "osal/ThreadFunctions.h"
#include "tracing/Tracing.h"
#include "utility/GenericError.h"
#include "midi/IMidiAPI.h"
#include "midi/IMidiInDevice.h"
#include "midi/IMidiOutDevice.h"
#include "midi/MidiCapabilities.h"

Application::Application(int argc, char *argv[])
    : m_applicationName(argv[0])
    , m_commandLineArguments()
    , m_interrupted()
    , m_api()
    , m_deviceIn()
    , m_deviceOut()
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

void Application::OnMidiEvent(const midi::MidiEvent & event)
{
    TraceInfo(__FILE__, __LINE__, __func__, "event={}", event);
    switch (event.type)
    {
        case midi::MidiEventType::NoteOn:
        case midi::MidiEventType::NoteOff:
            m_deviceOut->SendMidiOutEvent(midi::MidiEvent(event.type, event.channel, static_cast<midi::Key>(event.key + 0), event.velocity, event.timestamp));
            m_deviceOut->SendMidiOutEvent(midi::MidiEvent(event.type, event.channel, static_cast<midi::Key>(event.key + 4), event.velocity, event.timestamp));
            m_deviceOut->SendMidiOutEvent(midi::MidiEvent(event.type, event.channel, static_cast<midi::Key>(event.key + 7), event.velocity, event.timestamp));
            // m_deviceOut->SendMidiOutEvent(midi::MidiEvent(event.type, event.channel, static_cast<midi::Key>(event.key + 12), event.velocity, event.timestamp));
            // m_deviceOut->SendMidiOutEvent(midi::MidiEvent(event.type, event.channel, static_cast<midi::Key>(event.key + 16), event.velocity, event.timestamp));
            // m_deviceOut->SendMidiOutEvent(midi::MidiEvent(event.type, event.channel, static_cast<midi::Key>(event.key + 21), event.velocity, event.timestamp));
            break;
        case midi::MidiEventType::ControlChange:
            break;
        case midi::MidiEventType::PitchBend:
            break;
        default:
            break;
    }
}

int Application::Run()
{
    tracing::SetDefaultTraceFilter(tracing::TraceCategory::Information | tracing::TraceCategory::Data | tracing::TraceCategory::FunctionEnter | tracing::TraceCategory::FunctionLeave);
    osal::SetThreadNameSelf("Main");
    osal::SetSignalHandler(osal::SignalType::Interrupt, std::bind(&Application::SignalHandler, this, std::placeholders::_1));

    m_api = midi::CreateAPI();
    if (!m_api->Initialize())
    {
        tracing::Tracing::Fatal(__FILE__, __LINE__, __func__, utility::GenericError("Cannot initialize MIDI API"));
    }

    auto numInputDevices = m_api->GetNumInputDevices();
    auto numOutputDevices = m_api->GetNumOutputDevices();
    TraceInfo(__FILE__, __LINE__, __func__, "MIDI in devices: ", numInputDevices);
    for (std::size_t i = 0; i < numInputDevices; ++i)
    {
        midi::MidiCapabilities capabilities;
        if (m_api->GetCapabilitiesForInputDevice(i, capabilities))
            TraceInfo(__FILE__, __LINE__, __func__, "MIDI in device {}: {}", i, capabilities.Name());
        else
            tracing::Tracing::Error(__FILE__, __LINE__, __func__, utility::GenericError("Error while retrieving info for MIDI in device {}", i));
    }
    TraceInfo(__FILE__, __LINE__, __func__, "MIDI out devices: ", numOutputDevices);
    for (std::size_t i = 0; i < numOutputDevices; ++i)
    {
        midi::MidiCapabilities capabilities;
        if (m_api->GetCapabilitiesForOutputDevice(i, capabilities))
            TraceInfo(__FILE__, __LINE__, __func__, "MIDI out device {}: {}", i, capabilities.Name());
        else
            tracing::Tracing::Error(__FILE__, __LINE__, __func__, utility::GenericError("Error while retrieving info for MIDI out device {}", i));
    }

    TraceInfo(__FILE__, __LINE__, __func__, "Open MIDI device");
    m_deviceIn = m_api->OpenInputDevice("ESI KeyControl 25 XT");
    // m_deviceIn = m_api->OpenInputDevice("AIO Midi");
    m_deviceOut = m_api->OpenOutputDevice("AIO Midi");
    m_deviceIn->SetEventCallback(std::bind(&Application::OnMidiEvent, this, std::placeholders::_1));
    m_deviceIn->Start();

    while (!m_interrupted)
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
    TraceInfo(__FILE__, __LINE__, __func__, "Shutting down");
    m_deviceIn->Stop();
    m_deviceIn.reset();
    m_deviceOut.reset();

    return 0;
}

void Application::SignalHandler(osal::SignalType signal)
{
    TraceInfo(__FILE__, __LINE__, __func__, "Caught signal {}", signal);
    if (signal == osal::SignalType::Interrupt)
        m_interrupted = true;
}

