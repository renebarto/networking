#include "GoogleTest.h"

#include <algorithm>
#include "osal/ThreadFunctions.h"
#include "network-osal/SocketAPIMock.h"
#include "tracing/ScopedTracing.h"
#include "tracing/Tracing.h"
#include "network/IPV4TCPServerSocket.h"
#include "Utility.h"

namespace network {

using ::testing::_;
using ::testing::Return;

class IPV4TCPServerSocketTest : public ::testing::Test
{
public:
    tracing::CategorySet<tracing::TraceCategory> m_savedTraceFilter;

    IPV4TCPServerSocketTest()
        : m_savedTraceFilter()
    {}

    void SetUp() override
    {
        m_savedTraceFilter = tracing::GetDefaultTraceFilter();
        tracing::SetDefaultTraceFilter(tracing::TraceCategory::Message | tracing::TraceCategory::Data);
        osal::SetThreadNameSelf("IPV4TCPServerSocketTest");
    }
    void TearDown() override
    {
        tracing::SetDefaultTraceFilter(m_savedTraceFilter);
    }
};

TEST_F(IPV4TCPServerSocketTest, Construct)
{
    testing::SocketAPIMock api;
    EXPECT_CALL(api, Open(SocketFamily::InternetV4, SocketType::Stream, _)).Times(0);
    EXPECT_CALL(api, Close(_)).Times(0);

    IPV4TCPServerSocket server(api, 8080, 1, std::chrono::seconds::max());
    EXPECT_FALSE(server.IsInitialized());
}

TEST_F(IPV4TCPServerSocketTest, Initialize)
{
    const int NumListeners = 1;
    testing::SocketAPIMock api;
    EXPECT_CALL(api, Open(SocketFamily::InternetV4, SocketType::Stream, _)).Times(1);
    EXPECT_CALL(api, Close(_)).Times(1);
    EXPECT_CALL(api, Bind(_, _, _)).Times(1);
    EXPECT_CALL(api, Listen(_, NumListeners)).Times(1);

    IPV4TCPServerSocket server(api, TestPort, NumListeners, std::chrono::seconds::max());
    EXPECT_TRUE(server.Initialize());
    EXPECT_TRUE(server.IsInitialized());
}

TEST_F(IPV4TCPServerSocketTest, InitializeTwiceSucceeds)
{
    const int NumListeners = 1;
    testing::SocketAPIMock api;
    EXPECT_CALL(api, Open(SocketFamily::InternetV4, SocketType::Stream, _)).Times(1);
    EXPECT_CALL(api, Close(_)).Times(1);
    EXPECT_CALL(api, Bind(_, _, _)).Times(1);
    EXPECT_CALL(api, Listen(_, NumListeners)).Times(1);

    IPV4TCPServerSocket server(api, TestPort, NumListeners, std::chrono::seconds::max());
    EXPECT_TRUE(server.Initialize());
    EXPECT_TRUE(server.IsInitialized());
    EXPECT_TRUE(server.Initialize());
    EXPECT_TRUE(server.IsInitialized());
}

TEST_F(IPV4TCPServerSocketTest, Uninitialize)
{
    const int NumListeners = 1;
    testing::SocketAPIMock api;
    EXPECT_CALL(api, Open(SocketFamily::InternetV4, SocketType::Stream, _)).Times(1);
    EXPECT_CALL(api, Close(_)).Times(1);
    EXPECT_CALL(api, Bind(_, _, _)).Times(1);
    EXPECT_CALL(api, Listen(_, NumListeners)).Times(1);

    IPV4TCPServerSocket server(api, TestPort, NumListeners, std::chrono::seconds::max());
    EXPECT_TRUE(server.Initialize());
    EXPECT_TRUE(server.IsInitialized());
    EXPECT_TRUE(server.Uninitialize());
    EXPECT_FALSE(server.IsInitialized());
}

TEST_F(IPV4TCPServerSocketTest, UninitializeIfNotInitializedFails)
{
    const int NumListeners = 1;
    testing::SocketAPIMock api;
    EXPECT_CALL(api, Open(SocketFamily::InternetV4, SocketType::Stream, _)).Times(0);
    EXPECT_CALL(api, Close(_)).Times(0);
    EXPECT_CALL(api, Bind(_, _, _)).Times(0);
    EXPECT_CALL(api, Listen(_, NumListeners)).Times(0);

    IPV4TCPServerSocket server(api, TestPort, NumListeners, std::chrono::seconds::max());
    EXPECT_FALSE(server.IsInitialized());
    EXPECT_FALSE(server.Uninitialize());
    EXPECT_FALSE(server.IsInitialized());
}

TEST_F(IPV4TCPServerSocketTest, AcceptNoTimeout)
{
    const int NumListeners = 1;
    testing::SocketAPIMock api;
    EXPECT_CALL(api, Open(SocketFamily::InternetV4, SocketType::Stream, _)).Times(1);
    EXPECT_CALL(api, Close(_)).Times(2);
    EXPECT_CALL(api, Bind(_, _, _)).Times(1);
    EXPECT_CALL(api, Listen(_, NumListeners)).Times(1);
    EXPECT_CALL(api, SetBlockingMode(_, false)).Times(0);
    EXPECT_CALL(api, SetBlockingMode(_, true)).Times(2);
    EXPECT_CALL(api, Accept(_, _, _)).WillOnce(Return(1));

    IPV4TCPServerSocket server(api, TestPort, NumListeners, std::chrono::seconds::max());
    EXPECT_TRUE(server.Initialize());
    EXPECT_TRUE(server.IsInitialized());
    IPV4TCPSocket client(api);
    IPV4EndPoint clientAddress;
    EXPECT_TRUE(server.Accept(client, clientAddress));
}

TEST_F(IPV4TCPServerSocketTest, AcceptWithTimeout)
{
    const int NumListeners = 1;
    testing::SocketAPIMock api;
    EXPECT_CALL(api, Open(SocketFamily::InternetV4, SocketType::Stream, _)).Times(1);
    EXPECT_CALL(api, Close(_)).Times(2);
    EXPECT_CALL(api, Bind(_, _, _)).Times(1);
    EXPECT_CALL(api, Listen(_, NumListeners)).Times(1);
    EXPECT_CALL(api, SetBlockingMode(_, false)).Times(1);
    EXPECT_CALL(api, SetBlockingMode(_, true)).Times(1);
    EXPECT_CALL(api, Accept(_, _, _)).WillOnce(Return(1));

    IPV4TCPServerSocket server(api, TestPort, NumListeners, std::chrono::seconds(1));
    EXPECT_TRUE(server.Initialize());
    EXPECT_TRUE(server.IsInitialized());
    IPV4TCPSocket client(api);
    IPV4EndPoint clientAddress;
    EXPECT_TRUE(server.Accept(client, clientAddress));
}

TEST_F(IPV4TCPServerSocketTest, AcceptWithTimeoutFails)
{
    const int NumListeners = 1;
    testing::SocketAPIMock api;
    EXPECT_CALL(api, Open(SocketFamily::InternetV4, SocketType::Stream, _)).Times(1);
    EXPECT_CALL(api, Close(_)).Times(1);
    EXPECT_CALL(api, Bind(_, _, _)).Times(1);
    EXPECT_CALL(api, Listen(_, NumListeners)).Times(1);
    EXPECT_CALL(api, SetBlockingMode(_, false)).Times(1);
    EXPECT_CALL(api, SetBlockingMode(_, true)).Times(1);
    EXPECT_CALL(api, Accept(_, _, _)).WillOnce(Return(-1));

    IPV4TCPServerSocket server(api, TestPort, NumListeners, std::chrono::seconds(1));
    EXPECT_TRUE(server.Initialize());
    EXPECT_TRUE(server.IsInitialized());
    IPV4TCPSocket client(api);
    IPV4EndPoint clientAddress;
    EXPECT_FALSE(server.Accept(client, clientAddress));
}

bool IPV4TCPSocketConnectThread()
{
    bool connected {};
    SCOPEDTRACE([] () { return "TCP Connect Send Recv thread"; }, [&]{
        return serialization::Serialize(connected);
    });

    SocketAPI api;

    IPV4TCPSocket clientSocket(api);
    clientSocket.Open();
    IPV4EndPoint serverAddress(IPV4Address::LocalHost, TestPort);

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    connected = clientSocket.Connect(serverAddress, 5000);
    EXPECT_TRUE(connected);
    if (connected)
    {
        TraceMessage(__FILE__, __LINE__, __func__, "Connected");
        const std::size_t BufferSize = 10;
        std::uint8_t bufferOut[BufferSize] = { 'H', 'e', 'l', 'l', 'o', 'W', 'o', 'r', 'l', 'd'};
        std::uint8_t bufferIn[BufferSize];
        std::size_t bytesSent = clientSocket.Send(bufferOut, BufferSize, 0);
        EXPECT_EQ(BufferSize, bytesSent);
        std::size_t bytesReceived = clientSocket.Receive(bufferIn, bytesSent, 0);
        EXPECT_EQ(BufferSize, bytesReceived);
        EXPECT_TRUE(std::equal(std::begin(bufferIn), std::end(bufferIn), std::begin(bufferOut)));
        // Make sure to close client before server ends, otherwise we'll end up in TIME_WAIT status
        clientSocket.Close();
    }

    return connected;
}

TEST_F(IPV4TCPServerSocketTest, ConnectAcceptSendReceiveTCP)
{
    bool accepted {};
    SCOPEDTRACE([] () { return "TCP Accept Send Recv thread"; }, [&]{
        return serialization::Serialize(accepted);
    });

    SocketAPI api;

    const int NumListeners = 1;
    IPV4TCPServerSocket server(api, TestPort, NumListeners, std::chrono::seconds(1));
    server.Initialize();
    core::TypedReturnThread<bool> connectorThread(IPV4TCPSocketConnectThread, "IPV4TCPSocketConnectThread");

    IPV4TCPSocket client(api);
    client.Open();
    IPV4EndPoint clientAddress;
    accepted = server.Accept(client, clientAddress);
    EXPECT_TRUE(accepted);
    if (accepted)
    {
        TraceMessage(__FILE__, __LINE__, __func__, "Accepted connection");
        const std::size_t BufferSize = 10;
        std::uint8_t buffer[BufferSize];
        std::size_t bytesReceived = client.Receive(buffer, BufferSize, 0);
        client.Send(buffer, bytesReceived, 0);
        // Wait for client to close connection, otherwise we'll end up in TIME_WAIT status
        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    }
    server.Close();
    connectorThread.WaitForDeath();
    bool connected = connectorThread.GetResult();
    EXPECT_TRUE(connected);
}

} // namespace network
