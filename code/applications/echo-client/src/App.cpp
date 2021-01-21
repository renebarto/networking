#include "App.h"

#include "tracing/Logging.h"
#include "tracing/Tracing.h"
#include "osal/Thread.h"
#include "network-osal/SocketAPI.h"
#include "utility/GenericError.h"
#include "network/IPV4TCPClient.h"

Application::Application(int argc, char *argv[])
    : m_applicationName(argv[0])
    , m_commandLineArguments()
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
    tracing::SetDefaultTraceFilter(tracing::TraceCategory::Message | tracing::TraceCategory::Data);
    osal::SetThreadNameSelf("Main");
    std::string serverIPAddress = "127.0.0.1";
    if (m_commandLineArguments.size() >= 1)
        serverIPAddress = m_commandLineArguments[0];
    network::IPV4EndPoint serverEndPoint(network::IPV4Address::Parse(serverIPAddress), 7);
    network::SocketAPI api;
    network::IPV4TCPClient client(api, serverEndPoint);
    if (client.Connect(1000))
    {
        std::vector<std::uint8_t> bufferOut;
        std::vector<std::uint8_t> bufferIn;
        network::IPV4EndPoint clientEndPoint;
        client.GetLocalAddress(clientEndPoint);

        std::size_t bytesToSend = 0;
        auto localAddressString = serialization::Serialize(clientEndPoint, 0);
        serialization::SerializeBinary(localAddressString, bufferOut, bytesToSend);
        if (client.Send(bufferOut.data(), bytesToSend, 0))
        {
            bufferIn.resize(50);
            std::size_t bufferSize = bufferIn.size();
            auto numBytesReceived = client.Receive(bufferIn.data(), bufferSize, 0);
            bufferIn.resize(numBytesReceived);
            TraceData(__FILE__, __LINE__, __func__, "Sent {} bytes: {}", bytesToSend, serialization::SerializeData(bufferOut.data(), numBytesReceived));
            TraceData(__FILE__, __LINE__, __func__, "Received {} bytes: {}", numBytesReceived, serialization::SerializeData(bufferIn.data(), numBytesReceived));
        }
        else
        {
            LogError(__FILE__, __LINE__, __func__, "Error while sending");
            client.Disconnect();
        }
    }
    else
    {
        tracing::Logging::Fatal(__FILE__, __LINE__, __func__, utility::GenericError("Cannot connect to server {}", serverEndPoint));
    }
    return 0;
}
