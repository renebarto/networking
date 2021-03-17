#include "GoogleTest.h"

#include "network/IPV6Socket.h"

#include "network-osal/SocketAPI.h"
#include "tracing/ScopedTracing.h"
#include "utility/GenericError.h"
#include "Utility.h"

namespace network {

class IPV6SocketTest : public ::testing::Test
{
public:
    tracing::CategorySet<tracing::TraceCategory> m_savedTraceFilter;

    IPV6SocketTest()
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

TEST_F(IPV6SocketTest, ConstructDefault)
{
    SocketAPI api;
    IPV6Socket target(api);
    EXPECT_EQ(InvalidHandleValue, target.GetHandle());
    EXPECT_FALSE(target.IsOpen());
    EXPECT_EQ(SocketFamily::Any, target.Family());
    EXPECT_EQ(SocketType::None, target.Type());
    EXPECT_EQ(SocketProtocol::IP, target.Protocol());
}

TEST_F(IPV6SocketTest, ConstructWithType)
{
    SocketAPI api;
    IPV6Socket target(api, SocketType::Datagram);
    EXPECT_EQ(InvalidHandleValue, target.GetHandle());
    EXPECT_FALSE(target.IsOpen());
    EXPECT_EQ(SocketFamily::InternetV6, target.Family());
    EXPECT_EQ(SocketType::Datagram, target.Type());
    EXPECT_EQ(SocketProtocol::IP, target.Protocol());
}

TEST_F(IPV6SocketTest, ConstructWithTypeAndProtocol)
{
    SocketAPI api;
    IPV6Socket target(api, SocketType::Datagram, SocketProtocol::ICMP);
    EXPECT_EQ(InvalidHandleValue, target.GetHandle());
    EXPECT_FALSE(target.IsOpen());
    EXPECT_EQ(SocketFamily::InternetV6, target.Family());
    EXPECT_EQ(SocketType::Datagram, target.Type());
    EXPECT_EQ(SocketProtocol::ICMP, target.Protocol());
}

TEST_F(IPV6SocketTest, ConstructMove)
{
    SocketAPI api;
    IPV6Socket target(api, SocketType::Datagram);
    target.Open();
    auto handle = target.GetHandle();

    IPV6Socket newSocket(std::move(target));
    EXPECT_EQ(InvalidHandleValue, target.GetHandle());
    EXPECT_FALSE(target.IsOpen());
    EXPECT_EQ(handle, newSocket.GetHandle());
    EXPECT_TRUE(newSocket.IsOpen());
    EXPECT_EQ(SocketFamily::InternetV6, target.Family());
    EXPECT_EQ(SocketType::Datagram, target.Type());
    EXPECT_EQ(SocketProtocol::IP, target.Protocol());
    EXPECT_EQ(SocketFamily::InternetV6, newSocket.Family());
    EXPECT_EQ(SocketType::Datagram, newSocket.Type());
    EXPECT_EQ(SocketProtocol::IP, newSocket.Protocol());
}

TEST_F(IPV6SocketTest, AssignMove)
{
    SocketAPI api;
    IPV6Socket target(api, SocketType::Datagram);
    target.Open();
    auto handle = target.GetHandle();

    IPV6Socket newSocket(api);
    newSocket = std::move(target);
    EXPECT_EQ(InvalidHandleValue, target.GetHandle());
    EXPECT_FALSE(target.IsOpen());
    EXPECT_EQ(handle, newSocket.GetHandle());
    EXPECT_TRUE(newSocket.IsOpen());
    EXPECT_EQ(SocketFamily::InternetV6, target.Family());
    EXPECT_EQ(SocketType::Datagram, target.Type());
    EXPECT_EQ(SocketProtocol::IP, target.Protocol());
    EXPECT_EQ(SocketFamily::InternetV6, newSocket.Family());
    EXPECT_EQ(SocketType::Datagram, newSocket.Type());
    EXPECT_EQ(SocketProtocol::IP, newSocket.Protocol());
}

TEST_F(IPV6SocketTest, GetSetHandle)
{
    SocketAPI api;
    IPV6Socket target(api);
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

TEST_F(IPV6SocketTest, Open)
{
    SocketAPI api;
    IPV6Socket target(api, SocketType::Stream);
    target.Open();
    EXPECT_TRUE(target.IsOpen());
    EXPECT_EQ(SocketFamily::InternetV6, target.Family());
    EXPECT_EQ(SocketType::Stream, target.Type());
    EXPECT_EQ(SocketProtocol::IP, target.Protocol());
}

TEST_F(IPV6SocketTest, OpenAndReOpen)
{
    SocketAPI api;
    IPV6Socket target(api, SocketType::Stream);
    target.Open();
    EXPECT_TRUE(target.IsOpen());
    EXPECT_EQ(SocketFamily::InternetV6, target.Family());
    EXPECT_EQ(SocketType::Stream, target.Type());
    EXPECT_EQ(SocketProtocol::IP, target.Protocol());
    target.Open();
    EXPECT_TRUE(target.IsOpen());
    EXPECT_EQ(SocketFamily::InternetV6, target.Family());
    EXPECT_EQ(SocketType::Stream, target.Type());
    EXPECT_EQ(SocketProtocol::IP, target.Protocol());
}

TEST_F(IPV6SocketTest, Close)
{
    SocketAPI api;
    IPV6Socket target(api, SocketType::Stream);
    target.Open();
    EXPECT_TRUE(target.IsOpen());
    target.Close();
    EXPECT_FALSE(target.IsOpen());
    EXPECT_EQ(SocketFamily::InternetV6, target.Family());
    EXPECT_EQ(SocketType::Stream, target.Type());
    EXPECT_EQ(SocketProtocol::IP, target.Protocol());
}

TEST_F(IPV6SocketTest, GetOptionWithLevel)
{
    SocketAPI api;
    IPV6Socket target(api, SocketType::Datagram);
    target.Open();
    socklen_t size = static_cast<socklen_t>(sizeof(int));
    int value;
    target.GetSocketOptionWithLevel(SocketOptionLevel::Socket, SocketOption::Broadcast, &value, &size);
    EXPECT_FALSE(value != 0);
}

TEST_F(IPV6SocketTest, SetOptionWithLevel)
{
    SocketAPI api;
    IPV6Socket target(api, SocketType::Datagram);
    target.Open();
    EXPECT_FALSE(target.GetSocketOptionBool(SocketOption::Broadcast));
    socklen_t size = static_cast<socklen_t>(sizeof(int));
    int value = 1;
    int actual;
    target.SetSocketOptionWithLevel(SocketOptionLevel::Socket, SocketOption::Broadcast, &value, size);
    target.GetSocketOptionWithLevel(SocketOptionLevel::Socket, SocketOption::Broadcast, &actual, &size);
    EXPECT_EQ(value, actual);
}

TEST_F(IPV6SocketTest, GetOption)
{
    SocketAPI api;
    IPV6Socket target(api, SocketType::Datagram);
    target.Open();
    socklen_t size = static_cast<socklen_t>(sizeof(int));
    int value;
    target.GetSocketOption(SocketOption::Broadcast, &value, &size);
    EXPECT_FALSE(value != 0);
}

TEST_F(IPV6SocketTest, SetOption)
{
    SocketAPI api;
    IPV6Socket target(api, SocketType::Datagram);
    target.Open();
    EXPECT_FALSE(target.GetSocketOptionBool(SocketOption::Broadcast));
    socklen_t size = static_cast<socklen_t>(sizeof(int));
    int value = 1;
    int actual;
    target.SetSocketOption(SocketOption::Broadcast, &value, size);
    target.GetSocketOption(SocketOption::Broadcast, &actual, &size);
    EXPECT_EQ(value, actual);
}

TEST_F(IPV6SocketTest, GetSocketOptionBool)
{
    SocketAPI api;
    IPV6Socket target(api, SocketType::Datagram);
    target.Open();
    EXPECT_FALSE(target.GetSocketOptionBool(SocketOption::Broadcast));
}

TEST_F(IPV6SocketTest, SetSocketOptionBool)
{
    SocketAPI api;
    IPV6Socket target(api, SocketType::Datagram);
    target.Open();
    EXPECT_FALSE(target.GetSocketOptionBool(SocketOption::Broadcast));
    target.SetSocketOptionBool(SocketOption::Broadcast, true);
    EXPECT_TRUE(target.GetSocketOptionBool(SocketOption::Broadcast));
}

TEST_F(IPV6SocketTest, GetSocketOptionInt)
{
    SocketAPI api;
    IPV6Socket target(api, SocketType::Datagram);
    target.Open();
    EXPECT_EQ(0, target.GetSocketOptionInt(SocketOption::Broadcast));
}

TEST_F(IPV6SocketTest, SetSocketOptionInt)
{
    int enableBroadcast = 1;
    SocketAPI api;
    IPV6Socket target(api, SocketType::Datagram);
    target.Open();
    EXPECT_FALSE(target.GetSocketOptionInt(SocketOption::Broadcast));
    target.SetSocketOptionInt(SocketOption::Broadcast, enableBroadcast);
    EXPECT_EQ(enableBroadcast, target.GetSocketOptionInt(SocketOption::Broadcast));
}

TEST_F(IPV6SocketTest, GetBroadcastOption)
{
    SocketAPI api;
    IPV6Socket target(api, SocketType::Datagram);
    target.Open();
    EXPECT_FALSE(target.GetBroadcastOption());
}

TEST_F(IPV6SocketTest, SetBroadcastOption)
{
    SocketAPI api;
    IPV6Socket target(api, SocketType::Datagram);
    target.Open();
    EXPECT_FALSE(target.GetBroadcastOption());
    target.SetBroadcastOption(true);
    EXPECT_TRUE(target.GetBroadcastOption());
}

TEST_F(IPV6SocketTest, GetBlockingMode)
{
    SocketAPI api;
    IPV6Socket target(api, SocketType::Stream);
    target.Open();
    EXPECT_TRUE(target.GetBlockingMode());
}

TEST_F(IPV6SocketTest, SetBlockingMode)
{
    SocketAPI api;
    IPV6Socket target(api, SocketType::Stream);
    target.Open();
    EXPECT_TRUE(target.GetBlockingMode());
    target.SetBlockingMode(false);
    EXPECT_FALSE(target.GetBlockingMode());
    target.SetBlockingMode(true);
    EXPECT_TRUE(target.GetBlockingMode());
}

TEST_F(IPV6SocketTest, GetReuseAddress)
{
    SocketAPI api;
    IPV6Socket target(api, SocketType::Datagram);
    target.Open();
    EXPECT_FALSE(target.GetReuseAddress());
}

TEST_F(IPV6SocketTest, SetReuseAddress)
{
    SocketAPI api;
    IPV6Socket target(api, SocketType::Datagram);
    target.Open();
    EXPECT_FALSE(target.GetReuseAddress());
    target.SetReuseAddress(true);
    EXPECT_TRUE(target.GetReuseAddress());
    target.SetReuseAddress(false);
    EXPECT_FALSE(target.GetReuseAddress());
}

TEST_F(IPV6SocketTest, GetReceiveTimeout)
{
    SocketAPI api;
    IPV6Socket target(api, SocketType::Datagram);
    target.Open();
    std::chrono::milliseconds timeout(0);
    EXPECT_EQ(timeout, target.GetReceiveTimeout());
}

TEST_F(IPV6SocketTest, SetReceiveTimeout)
{
    SocketAPI api;
    IPV6Socket target(api, SocketType::Datagram);
    target.Open();
    std::chrono::milliseconds timeout(0);
    std::chrono::milliseconds timeoutNew(1000);
    EXPECT_EQ(timeout, target.GetReceiveTimeout());
    target.SetReceiveTimeout(timeoutNew);
    EXPECT_EQ(timeoutNew, target.GetReceiveTimeout());
    target.SetReceiveTimeout(timeout);
    EXPECT_EQ(timeout, target.GetReceiveTimeout());
}

TEST_F(IPV6SocketTest, GetSendTimeout)
{
    SocketAPI api;
    IPV6Socket target(api, SocketType::Datagram);
    target.Open();
    std::chrono::milliseconds timeout(0);
    EXPECT_EQ(timeout, target.GetSendTimeout());
}

TEST_F(IPV6SocketTest, SetSendTimeout)
{
    SocketAPI api;
    IPV6Socket target(api, SocketType::Datagram);
    target.Open();
    std::chrono::milliseconds timeout(0);
    std::chrono::milliseconds timeoutNew(1000);
    EXPECT_EQ(timeout, target.GetSendTimeout());
    target.SetSendTimeout(timeoutNew);
    EXPECT_EQ(timeoutNew, target.GetSendTimeout());
    target.SetSendTimeout(timeout);
    EXPECT_EQ(timeout, target.GetSendTimeout());
}

bool IPV6SocketTCPAcceptThread()
{
    SocketAPI api;
    bool accepted {};
    SCOPEDTRACE([] () { return "TCP Accept Send Recv thread"; }, [&]{
        return serialization::Serialize(accepted);
    });
    IPV6Socket acceptorSocket(api,SocketType::Stream);
    acceptorSocket.Open();

    acceptorSocket.Bind(IPV6EndPoint(TestPort));
    acceptorSocket.Listen(1);
    IPV6Socket newSocket(api);
    IPV6EndPoint clientAddress;
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

TEST_F(IPV6SocketTest, ConnectAcceptSendReceiveTCP)
{
    SocketAPI api;
    IPV6Socket clientSocket(api, SocketType::Stream);
    clientSocket.Open();
    IPV6EndPoint serverAddress(IPV6Address::LocalHost, TestPort);

    core::threading::TypedReturnThread<bool> acceptorThread(IPV6SocketTCPAcceptThread);
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

bool IPV6SocketUDPServerThread()
{
    SocketAPI api;
    bool ok {};
    try
    {
        SCOPEDTRACE([] () { return "UDP Send Recv thread"; }, [&]{
            return serialization::Serialize(ok);
        });
        IPV6Socket serverSocket(api, SocketType::Datagram);
        serverSocket.Open();
        IPV6EndPoint serverAddress(TestPort);

        serverSocket.Bind(serverAddress);

        const std::size_t Size = 10;
        ByteBuffer buffer;
        IPV6EndPoint clientAddress;
        std::size_t bytesReceived = serverSocket.ReceiveBufferFrom(clientAddress, buffer, Size);
        EXPECT_EQ(Size, bytesReceived);
        ok = serverSocket.SendBufferTo(clientAddress, buffer);
    }
    catch (std::exception & e)
    {
        tracing::Tracing::Error(__FILE__, __LINE__, __func__, utility::GenericError(e.what()));
        throw;
    }
    return ok;
}

TEST_F(IPV6SocketTest, SendReceiveUDPConnected)
{
    SocketAPI api;
    IPV6Socket clientSocket(api, SocketType::Datagram);
    clientSocket.Open();
    IPV6EndPoint serverAddress(IPV6Address::LocalHost, TestPort);

    core::threading::TypedReturnThread<bool> serverThread(IPV6SocketUDPServerThread);
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

TEST_F(IPV6SocketTest, SendReceiveUDPConnectionless)
{
    SocketAPI api;
    IPV6Socket clientSocket(api, SocketType::Datagram);
    clientSocket.Open();
    IPV6EndPoint serverAddress(IPV6Address::LocalHost, TestPort);

    core::threading::TypedReturnThread<bool> serverThread(IPV6SocketUDPServerThread);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    const std::size_t Size = 10;
    ByteBuffer bufferOut = { 'H', 'e', 'l', 'l', 'o', 'W', 'o', 'r', 'l', 'd'};
    ByteBuffer bufferIn;
    clientSocket.SendBufferTo(serverAddress, bufferOut);
    IPV6EndPoint peerAddress;
    std::size_t bytesReceived = clientSocket.ReceiveBufferFrom(peerAddress, bufferIn, Size);
    EXPECT_EQ(Size, bytesReceived);
    EXPECT_EQ(serverAddress, peerAddress);
    EXPECT_TRUE(std::equal(bufferIn.begin(), bufferIn.end(), bufferOut.begin()));
    serverThread.WaitForDeath();
    bool success = serverThread.GetResult();
    EXPECT_TRUE(success);
}

} // namespace network