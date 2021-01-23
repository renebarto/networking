#include "GoogleTest.h"

#include "network/IPV4Socket.h"

#include "network-osal/SocketAPI.h"
#include "tracing/ScopedTracing.h"
#include "tracing/Tracing.h"
#include "Utility.h"

namespace network {

class IPV4SocketTest : public ::testing::Test
{
public:
    tracing::CategorySet<tracing::TraceCategory> m_savedTraceFilter;

    IPV4SocketTest()
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

TEST_F(IPV4SocketTest, ConstructDefault)
{
    SocketAPI api;
    IPV4Socket target(api);
    EXPECT_EQ(InvalidHandleValue, target.GetHandle());
    EXPECT_FALSE(target.IsOpen());
    EXPECT_EQ(SocketFamily::Any, target.Family());
    EXPECT_EQ(SocketType::None, target.Type());
    EXPECT_EQ(SocketProtocol::IP, target.Protocol());
}

TEST_F(IPV4SocketTest, ConstructWithType)
{
    SocketAPI api;
    IPV4Socket target(api, SocketType::Datagram);
    EXPECT_EQ(InvalidHandleValue, target.GetHandle());
    EXPECT_FALSE(target.IsOpen());
    EXPECT_EQ(SocketFamily::InternetV4, target.Family());
    EXPECT_EQ(SocketType::Datagram, target.Type());
    EXPECT_EQ(SocketProtocol::IP, target.Protocol());
}

TEST_F(IPV4SocketTest, ConstructWithTypeAndProtocol)
{
    SocketAPI api;
    IPV4Socket target(api, SocketType::Datagram, SocketProtocol::ICMP);
    EXPECT_EQ(InvalidHandleValue, target.GetHandle());
    EXPECT_FALSE(target.IsOpen());
    EXPECT_EQ(SocketFamily::InternetV4, target.Family());
    EXPECT_EQ(SocketType::Datagram, target.Type());
    EXPECT_EQ(SocketProtocol::ICMP, target.Protocol());
}

TEST_F(IPV4SocketTest, ConstructMove)
{
    SocketAPI api;
    IPV4Socket target(api, SocketType::Datagram);
    target.Open();
    auto handle = target.GetHandle();

    IPV4Socket newSocket(std::move(target));
    EXPECT_EQ(InvalidHandleValue, target.GetHandle());
    EXPECT_FALSE(target.IsOpen());
    EXPECT_EQ(handle, newSocket.GetHandle());
    EXPECT_TRUE(newSocket.IsOpen());
    EXPECT_EQ(SocketFamily::InternetV4, target.Family());
    EXPECT_EQ(SocketType::Datagram, target.Type());
    EXPECT_EQ(SocketProtocol::IP, target.Protocol());
    EXPECT_EQ(SocketFamily::InternetV4, newSocket.Family());
    EXPECT_EQ(SocketType::Datagram, newSocket.Type());
    EXPECT_EQ(SocketProtocol::IP, newSocket.Protocol());
}

TEST_F(IPV4SocketTest, AssignMove)
{
    SocketAPI api;
    IPV4Socket target(api, SocketType::Datagram);
    target.Open();
    auto handle = target.GetHandle();

    IPV4Socket newSocket(api);
    newSocket = std::move(target);
    EXPECT_EQ(InvalidHandleValue, target.GetHandle());
    EXPECT_FALSE(target.IsOpen());
    EXPECT_EQ(handle, newSocket.GetHandle());
    EXPECT_TRUE(newSocket.IsOpen());
    EXPECT_EQ(SocketFamily::InternetV4, target.Family());
    EXPECT_EQ(SocketType::Datagram, target.Type());
    EXPECT_EQ(SocketProtocol::IP, target.Protocol());
    EXPECT_EQ(SocketFamily::InternetV4, newSocket.Family());
    EXPECT_EQ(SocketType::Datagram, newSocket.Type());
    EXPECT_EQ(SocketProtocol::IP, newSocket.Protocol());
}

TEST_F(IPV4SocketTest, GetSetHandle)
{
    SocketAPI api;
    IPV4Socket target(api);
    EXPECT_EQ(InvalidHandleValue, target.GetHandle());
    EXPECT_FALSE(target.IsOpen());

    SocketHandle handle = 1234;
    target.SetHandle(handle);
    EXPECT_EQ(handle, target.GetHandle());
    EXPECT_TRUE(target.IsOpen());

    target.SetHandle(InvalidHandleValue);
    EXPECT_EQ(InvalidHandleValue, target.GetHandle());
    EXPECT_FALSE(target.IsOpen());
}

TEST_F(IPV4SocketTest, Open)
{
    SocketAPI api;
    IPV4Socket target(api, SocketType::Stream);
    target.Open();
    EXPECT_TRUE(target.IsOpen());
    EXPECT_EQ(SocketFamily::InternetV4, target.Family());
    EXPECT_EQ(SocketType::Stream, target.Type());
    EXPECT_EQ(SocketProtocol::IP, target.Protocol());
}

TEST_F(IPV4SocketTest, OpenAndReOpen)
{
    SocketAPI api;
    IPV4Socket target(api, SocketType::Stream);
    target.Open();
    EXPECT_TRUE(target.IsOpen());
    EXPECT_EQ(SocketFamily::InternetV4, target.Family());
    EXPECT_EQ(SocketType::Stream, target.Type());
    EXPECT_EQ(SocketProtocol::IP, target.Protocol());
    target.Open();
    EXPECT_TRUE(target.IsOpen());
    EXPECT_EQ(SocketFamily::InternetV4, target.Family());
    EXPECT_EQ(SocketType::Stream, target.Type());
    EXPECT_EQ(SocketProtocol::IP, target.Protocol());
}

TEST_F(IPV4SocketTest, Close)
{
    SocketAPI api;
    IPV4Socket target(api, SocketType::Stream);
    target.Open();
    EXPECT_TRUE(target.IsOpen());
    target.Close();
    EXPECT_FALSE(target.IsOpen());
    EXPECT_EQ(SocketFamily::InternetV4, target.Family());
    EXPECT_EQ(SocketType::Stream, target.Type());
    EXPECT_EQ(SocketProtocol::IP, target.Protocol());
}

TEST_F(IPV4SocketTest, GetOptionWithLevel)
{
    SocketAPI api;
    IPV4Socket target(api, SocketType::Datagram);
    target.Open();
    socklen_t size = static_cast<socklen_t>(sizeof(int));
    int value;
    target.GetSocketOptionWithLevel(SocketOptionLevel::Socket, SocketOption::Broadcast, &value, &size);
    EXPECT_FALSE(value != 0);
}

TEST_F(IPV4SocketTest, SetOptionWithLevel)
{
    SocketAPI api;
    IPV4Socket target(api, SocketType::Datagram);
    target.Open();
    EXPECT_FALSE(target.GetSocketOptionBool(SocketOption::Broadcast));
    socklen_t size = static_cast<socklen_t>(sizeof(int));
    int value = 1;
    int actual;
    target.SetSocketOptionWithLevel(SocketOptionLevel::Socket, SocketOption::Broadcast, &value, size);
    target.GetSocketOptionWithLevel(SocketOptionLevel::Socket, SocketOption::Broadcast, &actual, &size);
    EXPECT_EQ(value, actual);
}

TEST_F(IPV4SocketTest, GetOption)
{
    SocketAPI api;
    IPV4Socket target(api, SocketType::Datagram);
    target.Open();
    socklen_t size = static_cast<socklen_t>(sizeof(int));
    int value;
    target.GetSocketOption(SocketOption::Broadcast, &value, &size);
    EXPECT_FALSE(value != 0);
}

TEST_F(IPV4SocketTest, SetOption)
{
    SocketAPI api;
    IPV4Socket target(api, SocketType::Datagram);
    target.Open();
    EXPECT_FALSE(target.GetSocketOptionBool(SocketOption::Broadcast));
    socklen_t size = static_cast<socklen_t>(sizeof(int));
    int value = 1;
    int actual;
    target.SetSocketOption(SocketOption::Broadcast, &value, size);
    target.GetSocketOption(SocketOption::Broadcast, &actual, &size);
    EXPECT_EQ(value, actual);
}

TEST_F(IPV4SocketTest, GetSocketOptionBool)
{
    SocketAPI api;
    IPV4Socket target(api, SocketType::Datagram);
    target.Open();
    EXPECT_FALSE(target.GetSocketOptionBool(SocketOption::Broadcast));
}

TEST_F(IPV4SocketTest, SetSocketOptionBool)
{
    SocketAPI api;
    IPV4Socket target(api, SocketType::Datagram);
    target.Open();
    EXPECT_FALSE(target.GetSocketOptionBool(SocketOption::Broadcast));
    target.SetSocketOptionBool(SocketOption::Broadcast, true);
    EXPECT_TRUE(target.GetSocketOptionBool(SocketOption::Broadcast));
}

TEST_F(IPV4SocketTest, GetSocketOptionInt)
{
    SocketAPI api;
    IPV4Socket target(api, SocketType::Datagram);
    target.Open();
    EXPECT_EQ(0, target.GetSocketOptionInt(SocketOption::Broadcast));
}

TEST_F(IPV4SocketTest, SetSocketOptionInt)
{
    int enableBroadcast = 1;
    SocketAPI api;
    IPV4Socket target(api, SocketType::Datagram);
    target.Open();
    EXPECT_FALSE(target.GetSocketOptionInt(SocketOption::Broadcast));
    target.SetSocketOptionInt(SocketOption::Broadcast, enableBroadcast);
    EXPECT_EQ(enableBroadcast, target.GetSocketOptionInt(SocketOption::Broadcast));
}

TEST_F(IPV4SocketTest, GetBroadcastOption)
{
    SocketAPI api;
    IPV4Socket target(api, SocketType::Datagram);
    target.Open();
    EXPECT_FALSE(target.GetBroadcastOption());
}

TEST_F(IPV4SocketTest, SetBroadcastOption)
{
    SocketAPI api;
    IPV4Socket target(api, SocketType::Datagram);
    target.Open();
    EXPECT_FALSE(target.GetBroadcastOption());
    target.SetBroadcastOption(true);
    EXPECT_TRUE(target.GetBroadcastOption());
}

TEST_F(IPV4SocketTest, GetBlockingMode)
{
    SocketAPI api;
    IPV4Socket target(api, SocketType::Stream);
    target.Open();
    EXPECT_TRUE(target.GetBlockingMode());
}

TEST_F(IPV4SocketTest, SetBlockingMode)
{
    SocketAPI api;
    IPV4Socket target(api, SocketType::Stream);
    target.Open();
    EXPECT_TRUE(target.GetBlockingMode());
    target.SetBlockingMode(false);
    EXPECT_FALSE(target.GetBlockingMode());
    target.SetBlockingMode(true);
    EXPECT_TRUE(target.GetBlockingMode());
}

TEST_F(IPV4SocketTest, GetReuseAddress)
{
    SocketAPI api;
    IPV4Socket target(api, SocketType::Datagram);
    target.Open();
    EXPECT_FALSE(target.GetReuseAddress());
}

TEST_F(IPV4SocketTest, SetReuseAddress)
{
    SocketAPI api;
    IPV4Socket target(api, SocketType::Datagram);
    target.Open();
    EXPECT_FALSE(target.GetReuseAddress());
    target.SetReuseAddress(true);
    EXPECT_TRUE(target.GetReuseAddress());
    target.SetReuseAddress(false);
    EXPECT_FALSE(target.GetReuseAddress());
}

TEST_F(IPV4SocketTest, GetReceiveTimeout)
{
    SocketAPI api;
    IPV4Socket target(api, SocketType::Datagram);
    target.Open();
    std::chrono::milliseconds timeout(0);
    EXPECT_EQ(timeout, target.GetReceiveTimeout());
}

TEST_F(IPV4SocketTest, SetReceiveTimeout)
{
    SocketAPI api;
    IPV4Socket target(api, SocketType::Datagram);
    target.Open();
    std::chrono::milliseconds timeout(0);
    std::chrono::milliseconds timeoutNew(1000);
    EXPECT_EQ(timeout, target.GetReceiveTimeout());
    target.SetReceiveTimeout(timeoutNew);
    EXPECT_EQ(timeoutNew, target.GetReceiveTimeout());
    target.SetReceiveTimeout(timeout);
    EXPECT_EQ(timeout, target.GetReceiveTimeout());
}

TEST_F(IPV4SocketTest, GetSendTimeout)
{
    SocketAPI api;
    IPV4Socket target(api, SocketType::Datagram);
    target.Open();
    std::chrono::milliseconds timeout(0);
    EXPECT_EQ(timeout, target.GetSendTimeout());
}

TEST_F(IPV4SocketTest, SetSendTimeout)
{
    SocketAPI api;
    IPV4Socket target(api, SocketType::Datagram);
    target.Open();
    std::chrono::milliseconds timeout(0);
    std::chrono::milliseconds timeoutNew(1000);
    EXPECT_EQ(timeout, target.GetSendTimeout());
    target.SetSendTimeout(timeoutNew);
    EXPECT_EQ(timeoutNew, target.GetSendTimeout());
    target.SetSendTimeout(timeout);
    EXPECT_EQ(timeout, target.GetSendTimeout());
}

bool IPV4SocketTCPAcceptThread()
{
    SocketAPI api;
    bool accepted {};
    SCOPEDTRACE([] () { return "TCP Accept SendBuffer Recv thread"; }, [&]{
        return serialization::Serialize(accepted);
    });
    IPV4Socket acceptorSocket(api, SocketType::Stream);
    acceptorSocket.Open();

    acceptorSocket.Bind(IPV4EndPoint(TestPort));
    acceptorSocket.Listen(1);
    IPV4Socket newSocket(api);
    IPV4EndPoint clientAddress;
    accepted = acceptorSocket.Accept(newSocket, clientAddress, 5000);
    const std::size_t Size = 10;
    ByteBuffer buffer;
    std::size_t bytesReceived = newSocket.ReceiveBuffer(buffer, Size, 0);
    EXPECT_EQ(Size, bytesReceived);
    EXPECT_TRUE(newSocket.SendBuffer(buffer, 0));
    // Wait for client to close connection, otherwise we'll end up in TIME_WAIT status
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    return accepted;
}

TEST_F(IPV4SocketTest, ConnectAcceptSendReceiveTCP)
{
    SocketAPI api;
    IPV4Socket clientSocket(api, SocketType::Stream);
    clientSocket.Open();
    IPV4EndPoint serverAddress(IPV4Address::LocalHost, TestPort);

    core::TypedReturnThread<bool> acceptorThread(IPV4SocketTCPAcceptThread);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    bool connected = clientSocket.Connect(serverAddress, 5000);
    EXPECT_TRUE(connected);
    if (connected)
    {
        const std::size_t Size = 10;
        ByteBuffer bufferOut = { 'H', 'e', 'l', 'l', 'o', 'W', 'o', 'r', 'l', 'd'};
        ByteBuffer bufferIn;
        EXPECT_TRUE(clientSocket.SendBuffer(bufferOut, 0));
        std::size_t bytesReceived = clientSocket.ReceiveBuffer(bufferIn, Size, 0);
        EXPECT_EQ(Size, bytesReceived);
        EXPECT_TRUE(std::equal(bufferIn.begin(), bufferIn.end(), bufferOut.begin()));
        // Make sure to close client before server ends, otherwise we'll end up in TIME_WAIT status
        clientSocket.Close();
    }
    acceptorThread.WaitForDeath();
    bool accepted = acceptorThread.GetResult();
    EXPECT_TRUE(accepted);
}

bool IPV4SocketUDPServerThread()
{
    SocketAPI api;
    bool ok {};
    SCOPEDTRACE([] () { return "UDP SendBuffer Recv thread"; }, [&]{
        return serialization::Serialize(ok);
    });
    IPV4Socket serverSocket(api, SocketType::Datagram);
    serverSocket.Open();
    IPV4EndPoint serverAddress(TestPort);

    serverSocket.Bind(serverAddress);

    const std::size_t Size = 10;
    ByteBuffer buffer;
    IPV4EndPoint clientAddress;
    std::size_t bytesReceived = serverSocket.ReceiveBufferFrom(clientAddress, buffer, Size);
    EXPECT_EQ(Size, bytesReceived);
    ok = serverSocket.SendBufferTo(clientAddress, buffer);

    return ok;
}

TEST_F(IPV4SocketTest, SendReceiveUDPConnected)
{
    SocketAPI api;
    IPV4Socket clientSocket(api,SocketType::Datagram);
    clientSocket.Open();
    IPV4EndPoint serverAddress(IPV4Address::LocalHost, TestPort);

    core::TypedReturnThread<bool> serverThread(IPV4SocketUDPServerThread);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    bool connected = clientSocket.Connect(serverAddress, 5000);
    EXPECT_TRUE(connected);
    if (connected)
    {
        const std::size_t Size = 10;
        ByteBuffer bufferOut = { 'H', 'e', 'l', 'l', 'o', 'W', 'o', 'r', 'l', 'd'};
        ByteBuffer bufferIn;
        EXPECT_TRUE(clientSocket.SendBuffer(bufferOut, 0));
        std::size_t bytesReceived = clientSocket.ReceiveBuffer(bufferIn, Size, 0);
        EXPECT_EQ(Size, bytesReceived);
        EXPECT_TRUE(std::equal(bufferIn.begin(), bufferIn.end(), bufferOut.begin()));
    }
    serverThread.WaitForDeath();
    bool success = serverThread.GetResult();
    EXPECT_TRUE(success);
}

TEST_F(IPV4SocketTest, SendReceiveUDPConnectionless)
{
    SocketAPI api;
    IPV4Socket clientSocket(api, SocketType::Datagram);
    clientSocket.Open();
    IPV4EndPoint serverAddress(IPV4Address::LocalHost, TestPort);

    core::TypedReturnThread<bool> serverThread(IPV4SocketUDPServerThread);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    const std::size_t Size = 10;
    ByteBuffer bufferOut = { 'H', 'e', 'l', 'l', 'o', 'W', 'o', 'r', 'l', 'd'};
    ByteBuffer bufferIn;
    clientSocket.SendBufferTo(serverAddress, bufferOut);
    IPV4EndPoint peerAddress;
    std::size_t bytesReceived = clientSocket.ReceiveBufferFrom(peerAddress, bufferIn, Size);
    EXPECT_EQ(Size, bytesReceived);
    EXPECT_EQ(serverAddress, peerAddress);
    EXPECT_TRUE(std::equal(bufferIn.begin(), bufferIn.end(), bufferOut.begin()));
    serverThread.WaitForDeath();
    bool success = serverThread.GetResult();
    EXPECT_TRUE(success);
}

} // namespace network