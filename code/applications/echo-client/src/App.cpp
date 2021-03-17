#include "App.h"

#include "tracing/Tracing.h"
#include "osal/ThreadFunctions.h"
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
    tracing::SetDefaultTraceFilter(tracing::TraceCategory::Information | tracing::TraceCategory::Data);
    osal::SetThreadNameSelf("Main");
    std::string serverIPAddress = "127.0.0.1";
    if (m_commandLineArguments.size() >= 1)
        serverIPAddress = m_commandLineArguments[0];
    network::IPV4EndPoint serverEndPoint(network::IPV4Address::Parse(serverIPAddress), 7);
    network::SocketAPI api;
    network::IPV4TCPClient client(api, serverEndPoint);
    if (client.Connect(std::chrono::seconds(1)))
    {
        network::ByteBuffer bufferOut;
        network::ByteBuffer bufferIn;
        network::IPV4EndPoint clientEndPoint;
        client.GetLocalAddress(clientEndPoint);

        std::size_t offset = 0;
        auto localAddressString = serialization::Serialize(clientEndPoint, 0);
        serialization::SerializeBinary(localAddressString, bufferOut, offset);
        if (client.SendBuffer(bufferOut, 0))
        {
            bufferIn.clear();
            auto numBytesReceived = client.ReceiveBuffer(bufferIn, offset, 0);
            TraceData(__FILE__, __LINE__, __func__, "Sent {} bytes: {}", offset, serialization::SerializeData(bufferOut));
            TraceData(__FILE__, __LINE__, __func__, "Received {} bytes: {}", numBytesReceived, serialization::SerializeData(bufferIn));
        }
        else
        {
            LogError(__FILE__, __LINE__, __func__, "Error while sending");
            client.Disconnect();
        }
    }
    else
    {
        tracing::Tracing::Fatal(__FILE__, __LINE__, __func__, utility::GenericError("Cannot connect to server {}", serverEndPoint));
    }
    return 0;
}
