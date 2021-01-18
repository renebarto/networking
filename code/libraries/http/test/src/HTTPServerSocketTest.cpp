#include "GoogleTest.h"

#include "network-osal/SocketAPI.h"
#include "tracing/ScopedTracing.h"
#include "tracing/Tracing.h"
#include "http/HTTPServerSocket.h"

namespace http {

class HTTPServerSocketTest : public ::testing::Test
{
public:
    tracing::CategorySet<tracing::TraceCategory> m_savedTraceFilter;

    HTTPServerSocketTest()
        : m_savedTraceFilter()
    {}

    void SetUp() override
    {
        m_savedTraceFilter = tracing::GetDefaultTraceFilter();
    }
    void TearDown() override
    {
        tracing::SetDefaultTraceFilter(m_savedTraceFilter);
    }
};

TEST(HTTPServerSocketTest, Construct)
{
    network::SocketAPI api;
    HTTPServerSocket server(api, 8080, 1, SocketBlocking::On);
    EXPECT_FALSE(server.IsInitialized());
}

TEST_F(HTTPServerSocketTest, Initialize)
{
    network::SocketAPI api;
    HTTPServerSocket server(api, 8080, 1, SocketBlocking::On);
    EXPECT_TRUE(server.Initialize());
    EXPECT_TRUE(server.IsInitialized());
}

TEST_F(HTTPServerSocketTest, InitializeTwice)
{
    network::SocketAPI api;
    HTTPServerSocket server(api, 8080, 1, SocketBlocking::On);
    EXPECT_TRUE(server.Initialize());
    EXPECT_TRUE(server.IsInitialized());
    EXPECT_FALSE(server.Initialize());
    EXPECT_TRUE(server.IsInitialized());
}

TEST_F(HTTPServerSocketTest, Uninitialize)
{
    network::SocketAPI api;
    HTTPServerSocket server(api, 8080, 1, SocketBlocking::On);
    EXPECT_TRUE(server.Initialize());
    EXPECT_TRUE(server.IsInitialized());
    EXPECT_TRUE(server.Uninitialize());
    EXPECT_FALSE(server.IsInitialized());
}

TEST_F(HTTPServerSocketTest, UninitializeIfNotInitialized)
{
    network::SocketAPI api;
    HTTPServerSocket server(api, 8080, 1, SocketBlocking::On);
    EXPECT_FALSE(server.IsInitialized());
    EXPECT_FALSE(server.Uninitialize());
    EXPECT_FALSE(server.IsInitialized());
}

// TEST_F(HTTPServerSocketTest, ConstructCopy)
// {
//     IPV4TCPSocket target;
//     auto handle = target.GetHandle();
//     IPV4TCPSocket newSocket(target);
//     EXPECT_EQ(handle, target.GetHandle());
//     EXPECT_TRUE(target.IsOpen());
//     EXPECT_EQ(handle, newSocket.GetHandle());
//     EXPECT_TRUE(newSocket.IsOpen());
//     EXPECT_EQ(SocketFamily::InternetV4, target.Family());
//     EXPECT_EQ(SocketType::Stream, target.Type());
//     EXPECT_EQ(SocketFamily::InternetV4, newSocket.Family());
//     EXPECT_EQ(SocketType::Stream, newSocket.Type());

//     // Do not close twice
//     newSocket.SetHandle(InvalidHandleValue);
// }

// TEST_F(HTTPServerSocketTest, ConstructMove)
// {
//     IPV4TCPSocket target;
//     auto handle = target.GetHandle();

//     IPV4TCPSocket newSocket(std::move(target));
//     EXPECT_EQ(InvalidHandleValue, target.GetHandle());
//     EXPECT_FALSE(target.IsOpen());
//     EXPECT_EQ(handle, newSocket.GetHandle());
//     EXPECT_TRUE(newSocket.IsOpen());
//     EXPECT_EQ(SocketFamily::Any, target.Family());
//     EXPECT_EQ(SocketType::None, target.Type());
//     EXPECT_EQ(SocketFamily::InternetV4, newSocket.Family());
//     EXPECT_EQ(SocketType::Stream, newSocket.Type());
// }

// TEST_F(HTTPServerSocketTest, AssignMove)
// {
//     IPV4TCPSocket target;
//     auto handle = target.GetHandle();

//     IPV4TCPSocket newSocket;
//     newSocket = std::move(target);
//     EXPECT_EQ(InvalidHandleValue, target.GetHandle());
//     EXPECT_FALSE(target.IsOpen());
//     EXPECT_EQ(handle, newSocket.GetHandle());
//     EXPECT_TRUE(newSocket.IsOpen());
//     EXPECT_EQ(SocketFamily::Any, target.Family());
//     EXPECT_EQ(SocketType::None, target.Type());
//     EXPECT_EQ(SocketFamily::InternetV4, newSocket.Family());
//     EXPECT_EQ(SocketType::Stream, newSocket.Type());
// }

// TEST_F(HTTPServerSocketTest, GetSetHandle)
// {
//     IPV4TCPSocket target;
//     target.Close();

//     SocketHandle handle = 1234;
//     target.SetHandle(handle);
//     EXPECT_EQ(handle, target.GetHandle());
//     EXPECT_TRUE(target.IsOpen());

//     target.SetHandle(InvalidHandleValue);
//     EXPECT_EQ(InvalidHandleValue, target.GetHandle());
//     EXPECT_FALSE(target.IsOpen());
// }

// TEST_F(HTTPServerSocketTest, Open)
// {
//     IPV4TCPSocket target;
//     target.Close();
//     target.Open();
//     EXPECT_TRUE(target.IsOpen());
//     EXPECT_EQ(SocketFamily::InternetV4, target.Family());
//     EXPECT_EQ(SocketType::Stream, target.Type());
// }

// TEST_F(HTTPServerSocketTest, Close)
// {
//     IPV4TCPSocket target;
//     target.Open();
//     EXPECT_TRUE(target.IsOpen());
//     target.Close();
//     EXPECT_FALSE(target.IsOpen());
//     EXPECT_EQ(SocketFamily::Any, target.Family());
//     EXPECT_EQ(SocketType::None, target.Type());
// }

// bool IPV4TCPSocketTCPAcceptThread()
// {
//     bool accepted {};
//     SCOPEDTRACE([] () { return "TCP Accept Send Recv thread"; }, [&]{
//         return serialization::Serialize(accepted);
//     });
//     IPV4TCPSocket acceptorSocket;

//     acceptorSocket.Bind(IPV4EndPoint(TestPort));
//     acceptorSocket.Listen(1);
//     IPV4TCPSocket newSocket;
//     IPV4EndPoint clientAddress;
//     accepted = acceptorSocket.Accept(newSocket, clientAddress, 5000);
//     const std::size_t BufferSize = 10;
//     std::uint8_t buffer[BufferSize];
//     std::size_t bytesReceived = newSocket.Receive(buffer, BufferSize, 0);
//     newSocket.Send(buffer, bytesReceived, 0);
//     // Wait for client to close connection, otherwise we'll end up in TIME_WAIT status
//     std::this_thread::sleep_for(std::chrono::milliseconds(1000));

//     return accepted;
// }

// TEST_F(HTTPServerSocketTest, ConnectAcceptSendReceiveTCP)
// {
//     IPV4TCPSocket clientSocket;
//     IPV4EndPoint serverAddress(IPV4Address::LocalHost, TestPort);

//     BoolReturnThread acceptorThread(IPV4TCPSocketTCPAcceptThread);
//     std::this_thread::sleep_for(std::chrono::milliseconds(1000));
//     bool connected = clientSocket.Connect(serverAddress, 5000);
//     EXPECT_TRUE(connected);
//     if (connected)
//     {
//         const std::size_t BufferSize = 10;
//         std::uint8_t bufferOut[BufferSize] = { 'H', 'e', 'l', 'l', 'o', 'W', 'o', 'r', 'l', 'd'};
//         std::uint8_t bufferIn[BufferSize];
//         EXPECT_TRUE(clientSocket.Send(bufferOut, BufferSize, 0));
//         std::size_t bytesReceived = clientSocket.Receive(bufferIn, BufferSize, 0);
//         EXPECT_EQ(BufferSize, bytesReceived);
//         EXPECT_TRUE(std::equal(std::begin(bufferIn), std::end(bufferIn), std::begin(bufferOut)));
//         // Make sure to close client before server ends, otherwise we'll end up in TIME_WAIT status
//         clientSocket.Close();
//     }
//     acceptorThread.WaitForDeath();
//     bool accepted = acceptorThread.GetResult();
//     EXPECT_TRUE(accepted);
// }

} // namespace http
