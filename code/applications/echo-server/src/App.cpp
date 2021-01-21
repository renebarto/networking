#include "App.h"

#include "tracing/Logging.h"
#include "tracing/Tracing.h"
#include "osal/Thread.h"
#include "core/Observable.h"
#include "network-osal/SocketAPI.h"
#include "utility/GenericError.h"
#include "network/IPV4TCPServer.h"

class IClosedConnectionCallback
{
public:
    virtual ~IClosedConnectionCallback() {};
    virtual void OnConnectionClosed(network::IIPV4TCPClientConnectionHandler * connection) = 0;
};

class TCPConnectionHandler
    : public network::IPV4TCPClientConnectionThread
    , public core::Observable<IClosedConnectionCallback>
{
public:
    bool m_active;
    std::vector<std::uint8_t> m_buffer;

    TCPConnectionHandler(network::ISocketAPI & api)
        : network::IPV4TCPClientConnectionThread(api)
        , m_active()
        , m_buffer(4096)
    {
        SCOPEDTRACE(nullptr, nullptr);
    }
    virtual ~TCPConnectionHandler()
    {
        SCOPEDTRACE(nullptr, nullptr);
    }
    void Run() override
    {
        SCOPEDTRACE(nullptr, nullptr);
        TraceMessage(__FILE__, __LINE__, __func__, "Starting {}", GetName());
        m_active = true;
        while (m_active)
        {
            auto bytesReceived = m_clientSocket.Receive(m_buffer.data(), m_buffer.size(), 0);
            if (bytesReceived != 0)
            {
                TraceData(__FILE__, __LINE__, __func__, serialization::SerializeData(m_buffer.data(), bytesReceived));
                if (!m_clientSocket.Send(m_buffer.data(), bytesReceived, 0))
                {
                    TraceMessage(__FILE__, __LINE__, __func__, "Could not send data correctly, received {}", bytesReceived);
                }
            }
            else
            {
                // If socket was closed, the connection has been dropped
                if (!m_clientSocket.IsOpen())
                {
                    TraceMessage(__FILE__, __LINE__, __func__, "Connection closed");
                    m_active = false;
                }
            }
        }
        TraceMessage(__FILE__, __LINE__, __func__, "Shutting down {}", GetName());
        ForAll([this] (IClosedConnectionCallback * listener) { listener->OnConnectionClosed(this); });
        m_clientSocket.Close();
    }
    void FlushThread() override
    {
        SCOPEDTRACE(nullptr, nullptr);
        m_active = false;
    }
};

using TCPConnectionHandlerPtr = std::shared_ptr<TCPConnectionHandler>;

class TCPServerHandler
    : public network::IPV4TCPServerThread
    , public IClosedConnectionCallback
{
public:
    TCPConnectionHandlerPtr m_connectionHandler;
    TCPConnectionHandlerPtr m_connectionHandlerClosed;

public:
    TCPServerHandler(network::ISocketAPI & api)
        : network::IPV4TCPServerThread(api)
        , m_connectionHandler()
    {        
    }
    
    bool Start(network::PortType port, int numListeners, network::SocketBlocking blocking) override
    {
        SCOPEDTRACE([&] () { return utility::FormatString(std::string("port={}, numListeners={}, blocking={}"), port, numListeners, blocking == network::SocketBlocking::On ? "On" : "Off"); }, 
                    nullptr);
        return IPV4TCPServerThread::Start(port, numListeners, blocking);
    }
    void DoConnectionCleanup() override
    {
        SCOPEDTRACE(nullptr, 
                    nullptr);
        if (m_connectionHandlerClosed)
        {
            TraceMessage(__FILE__, __LINE__, __func__, "Stop old connection");
            m_connectionHandlerClosed->Stop();
            TraceMessage(__FILE__, __LINE__, __func__, "Remove old connection");
            m_connectionHandlerClosed.reset();
        }
    }
    void ForceConnectionClose() override
    {
        SCOPEDTRACE(nullptr, 
                    nullptr);
        if (m_connectionHandler)
        {
            m_connectionHandler->FlushThread();
            TraceMessage(__FILE__, __LINE__, __func__, "Stop old connection");
            m_connectionHandler->Stop();
            TraceMessage(__FILE__, __LINE__, __func__, "Remove old connection");
            m_connectionHandler.reset();
        }
    }
    bool ReadyToAccept() override
    {
        SCOPEDTRACE(nullptr, 
                    nullptr);
        return (m_connectionHandler == nullptr);
    }
    bool OnAccepted(network::IPV4TCPSocket & clientSocket, const network::IPV4EndPoint & clientAddress) override
    {
        bool connectionAdded {};
        SCOPEDTRACE([&] () { return utility::FormatString(std::string("clientAddress={}"), clientAddress); }, 
                    [&] () { return utility::FormatString(std::string("connectionAdded={}"), connectionAdded);});
        if (m_connectionHandler == nullptr)
        {
            TraceMessage(__FILE__, __LINE__, __func__, "Add connection for {}", clientAddress);
            auto connectionHandler = std::make_shared<TCPConnectionHandler>(m_socketAPI);
            m_connectionHandler = connectionHandler;
            connectionHandler->Subscribe(this);
            connectionHandler->Start(clientSocket, clientAddress);
            connectionAdded = true;
        }
        else
        {
            clientSocket.Close();
        }
        return connectionAdded;
    }

    // Runs on connection handler thread
    void OnConnectionClosed(network::IIPV4TCPClientConnectionHandler * connection) override
    {
        SCOPEDTRACE([&] () { return utility::FormatString(std::string("connection={}"), connection); }, 
                    nullptr);
        TraceMessage(__FILE__, __LINE__, __func__, "Connection closed");
        m_connectionHandler->Unsubscribe(this);
        m_connectionHandlerClosed = m_connectionHandler;
        m_connectionHandler.reset();
    }
};

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
    TCPServerHandler serverHandler(api);
    network::IPV4TCPServer server(serverHandler);
    server.Start(port, 1, network::SocketBlocking::On);
    while (!m_interrupted)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }
    return result;
}
