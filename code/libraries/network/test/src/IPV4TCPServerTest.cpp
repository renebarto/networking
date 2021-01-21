#include "GoogleTest.h"

#include "osal/Thread.h"
#include "core/Observable.h"
#include "network-osal/SocketAPIMock.h"
#include "tracing/ScopedTracing.h"
#include "tracing/Tracing.h"
#include "network/IPV4TCPClient.h"
#include "network/IPV4TCPServer.h"

namespace network {

class IClosedConnectionCallback
{
public:
    virtual ~IClosedConnectionCallback() {};
    virtual void OnConnectionClosed(IIPV4TCPClientConnectionHandler * connection) = 0;
};

class TCPConnectionHandler
    : public IPV4TCPClientConnectionThread
    , public core::Observable<IClosedConnectionCallback>
{
public:
    bool m_abortThread;

    TCPConnectionHandler(ISocketAPI & api)
        : IPV4TCPClientConnectionThread(api)
        , m_abortThread()
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
        m_abortThread = false;
        while (!IsDying() && !m_abortThread)
        {
            TraceMessage(__FILE__, __LINE__, __func__, "Wait");
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
        }
        TraceMessage(__FILE__, __LINE__, __func__, "Shutting down {}", GetName());
        ForAll([this] (IClosedConnectionCallback * listener) { listener->OnConnectionClosed(this); });
    }
    void FlushThread() override
    {
        SCOPEDTRACE(nullptr, nullptr);
        m_abortThread = true;
    }
};

using TCPConnectionHandlerPtr = std::shared_ptr<TCPConnectionHandler>;

class IPV4TCPServerHandlerSingleConnection
    : public IPV4TCPServerThread
    , public IClosedConnectionCallback
{
public:
    TCPConnectionHandlerPtr m_connectionHandler;
    TCPConnectionHandlerPtr m_connectionHandlerClosed;
    bool m_connectionAdded;
    bool m_connectionRemoved;

public:
    IPV4TCPServerHandlerSingleConnection(ISocketAPI & api)
        : IPV4TCPServerThread(api)
        , m_connectionHandler()
        , m_connectionAdded()
        , m_connectionRemoved()
    {        
    }
    
    bool Start(PortType port, int numListeners, SocketBlocking blocking)
    {
        SCOPEDTRACE([&] () { return utility::FormatString(std::string("port={}, numListeners={}, blocking={}"), port, numListeners, blocking == SocketBlocking::On ? "On" : "Off"); }, 
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
    bool OnAccepted(IPV4TCPSocket & clientSocket, const IPV4EndPoint & clientAddress) override
    {
        bool connectionAdded {};
        SCOPEDTRACE([&] () { return utility::FormatString(std::string("clientAddress={}"), clientAddress); }, 
                    [&] () { return utility::FormatString(std::string("connectionAdded={}"), connectionAdded);});
        if (m_connectionHandler == nullptr)
        {
            TraceMessage(__FILE__, __LINE__, __func__, "Add connection for {}", clientAddress);
            auto connectionHandler = std::make_shared<TCPConnectionHandler>(m_socketAPI);
            m_connectionHandler = connectionHandler;
            m_connectionAdded = true;
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
    void OnConnectionClosed(IIPV4TCPClientConnectionHandler * connection) override
    {
        SCOPEDTRACE([&] () { return utility::FormatString(std::string("connection={}"), connection); }, 
                    nullptr);
        TraceMessage(__FILE__, __LINE__, __func__, "Connection closed");
        m_connectionRemoved = true;
        m_connectionHandler->Unsubscribe(this);
        m_connectionHandlerClosed = m_connectionHandler;
        m_connectionHandler.reset();
    }
};

class IPV4TCPServerHandlerLimitedConnections
    : private IPV4TCPServerThread
    , public IClosedConnectionCallback
{
private:
    std::size_t m_maxConnections;
    std::vector<TCPConnectionHandlerPtr> m_connectionHandlers;

public:
    IPV4TCPServerHandlerLimitedConnections(ISocketAPI & api, std::size_t maxConnections)
        : IPV4TCPServerThread(api)
        , m_maxConnections(maxConnections)
        , m_connectionHandlers()
    {        
    }
    
    void DoConnectionCleanup() override
    {
        SCOPEDTRACE(nullptr, 
                    nullptr);
    }
    void ForceConnectionClose() override
    {
        SCOPEDTRACE(nullptr, 
                    nullptr);
    }
    bool ReadyToAccept() override
    {
        SCOPEDTRACE(nullptr, 
                    nullptr);
        return (m_connectionHandlers.size() < m_maxConnections);
    }

    bool OnAccepted(IPV4TCPSocket & clientSocket, const IPV4EndPoint & clientAddress) override
    {
        bool connectionAdded {};
        SCOPEDTRACE([&] () { return utility::FormatString(std::string("clientAddress={}"), clientAddress); }, 
                    [&] () { return utility::FormatString(std::string("connectionAdded={}"), connectionAdded);});
        if (m_connectionHandlers.size() < m_maxConnections)
        {
            TraceMessage(__FILE__, __LINE__, __func__, "Add connection for {}", clientAddress);
            auto connectionHandler = std::make_shared<TCPConnectionHandler>(m_socketAPI);
            connectionHandler->Start(clientSocket, clientAddress);
            connectionHandler->Subscribe(this);
            m_connectionHandlers.push_back(connectionHandler);
            connectionAdded = true;
        }
        else
        {
            clientSocket.Close();
        }
        return connectionAdded;
    }

    void OnConnectionClosed(IIPV4TCPClientConnectionHandler * connection) override
    {
        SCOPEDTRACE([&] () { return utility::FormatString(std::string("connection={}"), connection); }, 
                    nullptr);
        TraceMessage(__FILE__, __LINE__, __func__, "Connection closed");
    }
};

static const std::uint16_t Port = 8080;
static const int NumListeners = 1;

class IPV4TCPServerTestSingleConnection : public ::testing::Test
{
public:
    tracing::CategorySet<tracing::TraceCategory> m_savedTraceFilter;

    IPV4TCPServerTestSingleConnection()
        : m_savedTraceFilter()
        , api()
        , serverHandler(api)
    {}

    void SetUp() override
    {
        m_savedTraceFilter = tracing::GetDefaultTraceFilter();
        tracing::SetDefaultTraceFilter(tracing::TraceCategory::Message | tracing::TraceCategory::Data | tracing::TraceCategory::All);
        osal::SetThreadNameSelf("IPV4TCPServerTestSingleConnection");
    }
    void TearDown() override
    {
        tracing::SetDefaultTraceFilter(m_savedTraceFilter);
    }

    SocketAPI api;
    IPV4TCPServerHandlerSingleConnection serverHandler;
};

TEST_F(IPV4TCPServerTestSingleConnection, Construct)
{
    IPV4TCPServer server(serverHandler);
    EXPECT_FALSE(server.IsStarted());
}

TEST_F(IPV4TCPServerTestSingleConnection, Start)
{
    IPV4TCPServer server(serverHandler);
    server.Start(Port, NumListeners, SocketBlocking::On);
    EXPECT_TRUE(server.IsStarted());
    EXPECT_FALSE(serverHandler.m_connectionAdded);
    EXPECT_FALSE(serverHandler.m_connectionRemoved);
}

TEST_F(IPV4TCPServerTestSingleConnection, StartStop)
{
    IPV4TCPServer server(serverHandler);
    server.Start(Port, NumListeners, SocketBlocking::On);
    EXPECT_TRUE(server.IsStarted());
    std::this_thread::sleep_for(std::chrono::seconds(2));
    server.Stop();
    EXPECT_FALSE(server.IsStarted());
    EXPECT_FALSE(serverHandler.m_connectionAdded);
    EXPECT_FALSE(serverHandler.m_connectionRemoved);
}

TEST_F(IPV4TCPServerTestSingleConnection, SingleConnectionAccepted)
{
    IPV4TCPServer server(serverHandler);
    server.Start(Port, NumListeners, SocketBlocking::On);
    EXPECT_TRUE(server.IsStarted());

    std::this_thread::sleep_for(std::chrono::seconds(1));

    IPV4EndPoint serverAddress(IPV4Address::LocalHost, Port);
    IPV4TCPClient client(api, serverAddress);
    EXPECT_TRUE(client.Connect(100));
    // client server socket will close after 1 second, so don't wait too long
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    EXPECT_TRUE(client.IsConnected());
    EXPECT_TRUE(serverHandler.m_connectionAdded);
    EXPECT_FALSE(serverHandler.m_connectionRemoved);
    client.Disconnect();
    std::this_thread::sleep_for(std::chrono::seconds(2));
    server.Stop();
    EXPECT_FALSE(server.IsStarted());
    EXPECT_TRUE(serverHandler.m_connectionAdded);
    EXPECT_TRUE(serverHandler.m_connectionRemoved);
}

TEST_F(IPV4TCPServerTestSingleConnection, SecondSimultaneousConnectionRefused)
{
    IPV4TCPServer server(serverHandler);
    server.Start(Port, NumListeners, SocketBlocking::On);
    EXPECT_TRUE(server.IsStarted());

    std::this_thread::sleep_for(std::chrono::seconds(2));

    IPV4EndPoint serverAddress(IPV4Address::LocalHost, Port);
    IPV4TCPClient client1(api, serverAddress);
    IPV4TCPClient client2(api, serverAddress);

    EXPECT_TRUE(client1.Connect(100));
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    EXPECT_FALSE(client2.Connect(100));
    // client server socket will close after 1 second, so don't wait too long
    std::this_thread::sleep_for(std::chrono::milliseconds(500));
    EXPECT_TRUE(serverHandler.m_connectionAdded);
    EXPECT_FALSE(serverHandler.m_connectionRemoved);
    EXPECT_TRUE(client1.IsConnected());
    EXPECT_FALSE(client2.IsConnected());
    client1.Disconnect();
    client2.Disconnect();
    std::this_thread::sleep_for(std::chrono::seconds(2));
    server.Stop();
    EXPECT_FALSE(server.IsStarted());
    EXPECT_TRUE(serverHandler.m_connectionAdded);
    EXPECT_TRUE(serverHandler.m_connectionRemoved);
}

} // namespace network
