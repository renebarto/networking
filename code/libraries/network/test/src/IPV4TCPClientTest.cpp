#include "GoogleTest.h"

#include "osal/Thread.h"
#include "tracing/ScopedTracing.h"
#include "network-osal/SocketAPI.h"
#include "network/IPV4TCPClient.h"
#include "network/IPV4TCPServerSocket.h"
#include "Utility.h"

namespace network {

static const std::uint16_t ServerPort = 8080;
static const int NumListeners = 1;

class IPV4TCPClientTest : public ::testing::Test
{
public:
    tracing::CategorySet<tracing::TraceCategory> m_savedTraceFilter;
    SocketAPI api;
    IPV4Address serverAddress;

    IPV4TCPClientTest()
        : m_savedTraceFilter()
        , api()
        , serverAddress(IPV4Address::LocalHost)
    {}

    void SetUp() override
    {
        m_savedTraceFilter = tracing::GetDefaultTraceFilter();
        tracing::SetDefaultTraceFilter(tracing::TraceCategory::Message | tracing::TraceCategory::Data);
        osal::SetThreadNameSelf("IPV4TCPClientTest");
    }
    void TearDown() override
    {
        tracing::SetDefaultTraceFilter(m_savedTraceFilter);
    }
};

TEST_F(IPV4TCPClientTest, ConstructEndPoint)
{
    IPV4EndPoint serverEndPoint(serverAddress, ServerPort);
    IPV4TCPClient client(api, serverEndPoint);
    EXPECT_EQ(serverEndPoint, client.ServerEndPoint());
}

TEST_F(IPV4TCPClientTest, ConstructAddressPort)
{
    IPV4TCPClient client(api, serverAddress, ServerPort);
    EXPECT_EQ(IPV4EndPoint(serverAddress, ServerPort), client.ServerEndPoint());
}

bool IPV4TCPSocketAcceptThread()
{
    bool accepted {};
    SCOPEDTRACE([] () { return "TCP Accept Send Recv thread"; }, [&]{
        return serialization::Serialize(accepted);
    });

    SocketAPI api;

    IPV4TCPServerSocket server(api, ServerPort, NumListeners, std::chrono::seconds(1));
    server.Initialize();

    IPV4TCPSocket client(api);
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

    return accepted;
}

TEST_F(IPV4TCPClientTest, ConnectAcceptSendReceiveTCP)
{
    core::TypedReturnThread<bool> acceptorThread(IPV4TCPSocketAcceptThread);

    bool connected {};
    SCOPEDTRACE([] () { return "TCP Connect Send Recv thread"; }, [&]{
        return serialization::Serialize(connected);
    });

    IPV4TCPClient client(api, serverAddress, ServerPort);

    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    connected = client.Connect(std::chrono::seconds(5));
    EXPECT_TRUE(connected);
    if (connected)
    {
        TraceMessage(__FILE__, __LINE__, __func__, "Connected");
        const std::size_t BufferSize = 10;
        std::uint8_t bufferOut[BufferSize] = { 'H', 'e', 'l', 'l', 'o', 'W', 'o', 'r', 'l', 'd'};
        std::uint8_t bufferIn[BufferSize];
        std::size_t bytesSent = client.Send(bufferOut, BufferSize, 0);
        EXPECT_EQ(BufferSize, bytesSent);
        std::size_t bytesReceived = client.Receive(bufferIn, BufferSize, 0);
        EXPECT_EQ(BufferSize, bytesReceived);
        EXPECT_TRUE(std::equal(std::begin(bufferIn), std::end(bufferIn), std::begin(bufferOut)));
        // Make sure to close client before server ends, otherwise we'll end up in TIME_WAIT status
        client.Disconnect();
    }

    acceptorThread.WaitForDeath();
    bool accepted = acceptorThread.GetResult();
    EXPECT_TRUE(accepted);
}

} // namespace network
