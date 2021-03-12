#include "App.h"

#include "tracing/Logging.h"
#include "tracing/Tracing.h"
#include "osal/ThreadFunctions.h"
#include "core/Observable.h"
#include "network-osal/SocketAPI.h"
#include "utility/GenericError.h"
#include "network/IPV4TCPServer.h"
#include "network/IPV4TCPServerHandler.h"

using namespace std::placeholders;

Application::Application(int argc, char *argv[])
    : m_applicationName(argv[0])
    , m_commandLineArguments()
    , m_interrupted()
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
    std::cerr << "Usage: " << m_applicationName << " <server port>" << std::endl;
}

void Application::SignalHandler(osal::SignalType signal)
{
    TraceMessage(__FILE__, __LINE__, __func__, "Caught signal {}", signal);
    if (signal == osal::SignalType::Interrupt)
        m_interrupted = true;
}

bool Application::DataCallback(const network::ByteBuffer & dataReceived, network::ByteBuffer & dataToSend)
{
    TraceMessage(__FILE__, __LINE__, __func__, "Data in: {}", serialization::SerializeData(dataReceived.data(), dataReceived.size()));
    dataToSend = dataReceived;
    return true;
}

int Application::Run()
{
    int result {};
    tracing::SetDefaultTraceFilter(tracing::TraceCategory::Message | tracing::TraceCategory::Data);
    osal::SetThreadNameSelf("Main");
    osal::SetSignalHandler(osal::SignalType::Interrupt, std::bind(&Application::SignalHandler, this, std::placeholders::_1));

    std::string serverPort = "7";
    if (m_commandLineArguments.size() >= 1)
        serverPort = m_commandLineArguments[0];
    network::PortType port;
    if (!serialization::Deserialize(serverPort, port))
    {
        port = 7;
        tracing::Logging::Error(__FILE__, __LINE__, __func__, utility::GenericError("Cannot parse port {}, falling back to default", serverPort, port));
    }

    network::SocketAPI api;
    network::IPV4TCPServerHandler serverHandler(api, std::bind(&Application::DataCallback, this, _1, _2));
    network::IPV4TCPServer server(serverHandler);
    server.Start(port, 1, std::chrono::seconds(2));
    while (!m_interrupted)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    server.Stop();
    return result;
}
