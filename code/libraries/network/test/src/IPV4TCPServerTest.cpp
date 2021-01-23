#include "GoogleTest.h"

#include "osal/Thread.h"
#include "core/Observable.h"
#include "network-osal/SocketAPIMock.h"
#include "tracing/ScopedTracing.h"
#include "tracing/Tracing.h"
#include "network/IPV4TCPClient.h"
#include "network/IPV4TCPServer.h"
#include "Utility.h"

namespace network {

class IClosedConnectionCallback
{
public:
    virtual ~IClosedConnectionCallback() {};
    virtual void OnConnectionClosed(IPV4TCPServerConnectionThread * connection) = 0;
};

class TCPConnectionHandler
    : public IPV4TCPServerConnectionThread
    , public core::Observable<IClosedConnectionCallback>
{
public:
    bool m_abortThread;

    TCPConnectionHandler(ISocketAPI & api)
        : IPV4TCPServerConnectionThread(api)
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
        SCOPEDTRACE([&] () { return utility::FormatString("port={}, numListeners={}, blocking={}", port, numListeners, blocking == SocketBlocking::On ? "On" : "Off"); }, 
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
        bool result {};
        SCOPEDTRACE(nullptr, 
                    [&] () { return utility::FormatString("result={}"), result);});
        result = (m_connectionHandler == nullptr);
        return result;
    }
    bool OnAccepted(IPV4TCPSocket && clientSocket, const IPV4EndPoint & clientAddress) override
    {
        bool connectionAdded {};
        SCOPEDTRACE([&] () { return utility::FormatString("clientAddress={}"), clientAddress); }, 
                    [&] () { return utility::FormatString("connectionAdded={}"), connectionAdded);});
        if (m_connectionHandler == nullptr)
        {
            TraceMessage(__FILE__, __LINE__, __func__, "Add connection for {}", clientAddress);
            auto connectionHandler = std::make_shared<TCPConnectionHandler>(m_socketAPI);
            m_connectionHandler = connectionHandler;
            m_connectionAdded = true;
            connectionHandler->Subscribe(this);
            connectionHandler->Start(std::move(clientSocket), clientAddress);
            connectionAdded = true;
        }
        else
        {
            clientSocket.Close();
        }
        return connectionAdded;
    }

    // Runs on connection handler thread
    void OnConnectionClosed(IPV4TCPServerConnectionThread * connection) override
    {
        SCOPEDTRACE([&] () { return utility::FormatString("connection={}"), connection); }, 
                    nullptr);
        TraceMessage(__FILE__, __LINE__, __func__, "Connection closed");
        m_connectionRemoved = true;
        m_connectionHandler->Unsubscribe(this);
        m_connectionHandlerClosed = m_connectionHandler;
        m_connectionHandler.reset();
    }
};

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
        tracing::SetDefaultTraceFilter(tracing::TraceCategory::Message | tracing::TraceCategory::Data);
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
    server.Start(TestPort, NumListeners, SocketBlocking::On);
    EXPECT_TRUE(server.IsStarted());
    EXPECT_FALSE(serverHandler.m_connectionAdded);
    EXPECT_FALSE(serverHandler.m_connectionRemoved);
}

TEST_F(IPV4TCPServerTestSingleConnection, StartStop)
{
    IPV4TCPServer server(serverHandler);
    server.Start(TestPort, NumListeners, SocketBlocking::On);
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
    server.Start(TestPort, NumListeners, SocketBlocking::On);
    EXPECT_TRUE(server.IsStarted());

    std::this_thread::sleep_for(std::chrono::seconds(1));

    IPV4EndPoint serverAddress(IPV4Address::LocalHost, TestPort);
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
    server.Start(TestPort, NumListeners, SocketBlocking::On);
    EXPECT_TRUE(server.IsStarted());

    std::this_thread::sleep_for(std::chrono::seconds(2));

    IPV4EndPoint serverAddress(IPV4Address::LocalHost, TestPort);
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
