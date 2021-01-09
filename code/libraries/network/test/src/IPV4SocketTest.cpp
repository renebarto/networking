#include "GoogleTest.h"

#include "network/IPV4Socket.h"

#include "tracing/ScopedTracing.h"
#include "tracing/Tracing.h"
#include "Utility.h"

namespace network {

class IPV4SocketTest : public ::testing::Test
{
public:
    IPV4SocketTest()
    {
        tracing::Tracing::SetTracingFunctions(
            nullptr, 
            [](tracing::TraceCategory /*category*/) { return false; });
    }
};

TEST_F(IPV4SocketTest, ConstructDefault)
{
    IPV4Socket target;
    EXPECT_EQ(InvalidHandleValue, target.GetHandle());
    EXPECT_FALSE(target.IsOpen());
    EXPECT_EQ(SocketFamily::Any, target.Family());
    EXPECT_EQ(SocketType::None, target.Type());
}

TEST_F(IPV4SocketTest, Construct)
{
    IPV4Socket target(SocketType::Datagram);
    EXPECT_NE(InvalidHandleValue, target.GetHandle());
    EXPECT_TRUE(target.IsOpen());
    EXPECT_EQ(SocketFamily::InternetV4, target.Family());
    EXPECT_EQ(SocketType::Datagram, target.Type());
}

TEST_F(IPV4SocketTest, ConstructCopy)
{
    IPV4Socket target(SocketType::Datagram);
    auto handle = target.GetHandle();
    IPV4Socket newSocket(target);
    EXPECT_EQ(handle, target.GetHandle());
    EXPECT_TRUE(target.IsOpen());
    EXPECT_EQ(handle, newSocket.GetHandle());
    EXPECT_TRUE(newSocket.IsOpen());
    EXPECT_EQ(SocketFamily::InternetV4, target.Family());
    EXPECT_EQ(SocketType::Datagram, target.Type());
    EXPECT_EQ(SocketFamily::InternetV4, newSocket.Family());
    EXPECT_EQ(SocketType::Datagram, newSocket.Type());

    // Do not close twice
    newSocket.SetHandle(InvalidHandleValue);
}

TEST_F(IPV4SocketTest, ConstructMove)
{
    IPV4Socket target(SocketType::Datagram);
    auto handle = target.GetHandle();

    IPV4Socket newSocket(std::move(target));
    EXPECT_EQ(InvalidHandleValue, target.GetHandle());
    EXPECT_FALSE(target.IsOpen());
    EXPECT_EQ(handle, newSocket.GetHandle());
    EXPECT_TRUE(newSocket.IsOpen());
    EXPECT_EQ(SocketFamily::Any, target.Family());
    EXPECT_EQ(SocketType::None, target.Type());
    EXPECT_EQ(SocketFamily::InternetV4, newSocket.Family());
    EXPECT_EQ(SocketType::Datagram, newSocket.Type());
}

TEST_F(IPV4SocketTest, AssignMove)
{
    IPV4Socket target(SocketType::Datagram);
    auto handle = target.GetHandle();

    IPV4Socket newSocket;
    newSocket = std::move(target);
    EXPECT_EQ(InvalidHandleValue, target.GetHandle());
    EXPECT_FALSE(target.IsOpen());
    EXPECT_EQ(handle, newSocket.GetHandle());
    EXPECT_TRUE(newSocket.IsOpen());
    EXPECT_EQ(SocketFamily::Any, target.Family());
    EXPECT_EQ(SocketType::None, target.Type());
    EXPECT_EQ(SocketFamily::InternetV4, newSocket.Family());
    EXPECT_EQ(SocketType::Datagram, newSocket.Type());
}

TEST_F(IPV4SocketTest, GetSetHandle)
{
    IPV4Socket target;
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
    IPV4Socket target;
    target.Open(SocketType::Stream);
    EXPECT_TRUE(target.IsOpen());
    EXPECT_EQ(SocketFamily::InternetV4, target.Family());
    EXPECT_EQ(SocketType::Stream, target.Type());
}

TEST_F(IPV4SocketTest, OpenAndReOpen)
{
    IPV4Socket target;
    target.Open(SocketType::Stream);
    EXPECT_TRUE(target.IsOpen());
    EXPECT_EQ(SocketFamily::InternetV4, target.Family());
    EXPECT_EQ(SocketType::Stream, target.Type());
    target.Open(SocketType::Datagram);
    EXPECT_TRUE(target.IsOpen());
    EXPECT_EQ(SocketFamily::InternetV4, target.Family());
    EXPECT_EQ(SocketType::Datagram, target.Type());
}

TEST_F(IPV4SocketTest, Close)
{
    IPV4Socket target;
    target.Open(SocketType::Stream);
    EXPECT_TRUE(target.IsOpen());
    target.Close();
    EXPECT_FALSE(target.IsOpen());
    EXPECT_EQ(SocketFamily::Any, target.Family());
    EXPECT_EQ(SocketType::None, target.Type());
}

TEST_F(IPV4SocketTest, GetOptionWithLevel)
{
    IPV4Socket target(SocketType::Datagram);
    socklen_t size = static_cast<socklen_t>(sizeof(int));
    int value;
    target.GetSocketOptionWithLevel(SocketOptionLevel::Socket, SocketOption::Broadcast, &value, &size);
    EXPECT_FALSE(value != 0);
}

TEST_F(IPV4SocketTest, SetOptionWithLevel)
{
    IPV4Socket target(SocketType::Datagram);
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
    IPV4Socket target(SocketType::Datagram);
    socklen_t size = static_cast<socklen_t>(sizeof(int));
    int value;
    target.GetSocketOption(SocketOption::Broadcast, &value, &size);
    EXPECT_FALSE(value != 0);
}

TEST_F(IPV4SocketTest, SetOption)
{
    IPV4Socket target(SocketType::Datagram);
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
    IPV4Socket target(SocketType::Datagram);
    EXPECT_FALSE(target.GetSocketOptionBool(SocketOption::Broadcast));
}

TEST_F(IPV4SocketTest, SetSocketOptionBool)
{
    IPV4Socket target(SocketType::Datagram);
    EXPECT_FALSE(target.GetSocketOptionBool(SocketOption::Broadcast));
    target.SetSocketOptionBool(SocketOption::Broadcast, true);
    EXPECT_TRUE(target.GetSocketOptionBool(SocketOption::Broadcast));
}

TEST_F(IPV4SocketTest, GetSocketOptionInt)
{
    IPV4Socket target(SocketType::Datagram);
    EXPECT_EQ(0, target.GetSocketOptionInt(SocketOption::Broadcast));
}

TEST_F(IPV4SocketTest, SetSocketOptionInt)
{
    int enableBroadcast = 1;
    IPV4Socket target(SocketType::Datagram);
    EXPECT_FALSE(target.GetSocketOptionInt(SocketOption::Broadcast));
    target.SetSocketOptionInt(SocketOption::Broadcast, enableBroadcast);
    EXPECT_EQ(enableBroadcast, target.GetSocketOptionInt(SocketOption::Broadcast));
}

TEST_F(IPV4SocketTest, GetBroadcastOption)
{
    IPV4Socket target(SocketType::Datagram);
    EXPECT_FALSE(target.GetBroadcastOption());
}

TEST_F(IPV4SocketTest, SetBroadcastOption)
{
    IPV4Socket target(SocketType::Datagram);
    EXPECT_FALSE(target.GetBroadcastOption());
    target.SetBroadcastOption(true);
    EXPECT_TRUE(target.GetBroadcastOption());
}

TEST_F(IPV4SocketTest, GetBlockingMode)
{
    IPV4Socket target(SocketType::Stream);
    EXPECT_TRUE(target.GetBlockingMode());
}

TEST_F(IPV4SocketTest, SetBlockingMode)
{
    IPV4Socket target(SocketType::Stream);
    EXPECT_TRUE(target.GetBlockingMode());
    target.SetBlockingMode(false);
    EXPECT_FALSE(target.GetBlockingMode());
    target.SetBlockingMode(true);
    EXPECT_TRUE(target.GetBlockingMode());
}

TEST_F(IPV4SocketTest, GetReuseAddress)
{
    IPV4Socket target(SocketType::Datagram);
    EXPECT_FALSE(target.GetReuseAddress());
}

TEST_F(IPV4SocketTest, SetReuseAddress)
{
    IPV4Socket target(SocketType::Datagram);
    EXPECT_FALSE(target.GetReuseAddress());
    target.SetReuseAddress(true);
    EXPECT_TRUE(target.GetReuseAddress());
    target.SetReuseAddress(false);
    EXPECT_FALSE(target.GetReuseAddress());
}

TEST_F(IPV4SocketTest, GetReceiveTimeout)
{
    IPV4Socket target(SocketType::Datagram);
    std::chrono::milliseconds timeout(0);
    EXPECT_EQ(timeout, target.GetReceiveTimeout());
}

TEST_F(IPV4SocketTest, SetReceiveTimeout)
{
    IPV4Socket target(SocketType::Datagram);
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
    IPV4Socket target(SocketType::Datagram);
    std::chrono::milliseconds timeout(0);
    EXPECT_EQ(timeout, target.GetSendTimeout());
}

TEST_F(IPV4SocketTest, SetSendTimeout)
{
    IPV4Socket target(SocketType::Datagram);
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
    bool accepted {};
    SCOPEDTRACE([] () { return "TCP Accept Send Recv thread"; }, [&]{
        return serialization::Serialize(accepted);
    });
    IPV4Socket acceptorSocket(SocketType::Stream);

    acceptorSocket.Bind(IPV4EndPoint(TestPort));
    acceptorSocket.Listen(1);
    IPV4Socket newSocket;
    IPV4EndPoint clientAddress;
    accepted = acceptorSocket.Accept(newSocket, clientAddress, 5000);
    const std::size_t BufferSize = 10;
    std::uint8_t buffer[BufferSize];
    std::size_t bytesReceived = newSocket.Receive(buffer, BufferSize, 0);
    newSocket.Send(buffer, bytesReceived, 0);

    return accepted;
}

TEST_F(IPV4SocketTest, ConnectAcceptSendReceiveTCP)
{
    IPV4Socket clientSocket(SocketType::Stream);
    IPV4EndPoint serverAddress(IPV4Address::LocalHost, TestPort);

    BoolReturnThread acceptorThread(IPV4SocketTCPAcceptThread);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    bool connected = clientSocket.Connect(serverAddress, 5000);
    EXPECT_TRUE(connected);
    if (connected)
    {
        const std::size_t BufferSize = 10;
        std::uint8_t bufferOut[BufferSize] = { 'H', 'e', 'l', 'l', 'o', 'W', 'o', 'r', 'l', 'd'};
        std::uint8_t bufferIn[BufferSize];
        EXPECT_TRUE(clientSocket.Send(bufferOut, BufferSize, 0));
        std::size_t bytesReceived = clientSocket.Receive(bufferIn, BufferSize, 0);
        EXPECT_EQ(BufferSize, bytesReceived);
        EXPECT_TRUE(std::equal(std::begin(bufferIn), std::end(bufferIn), std::begin(bufferOut)));
    }
    acceptorThread.WaitForDeath();
    bool accepted = acceptorThread.GetResult();
    EXPECT_TRUE(accepted);
}

bool IPV4SocketUDPServerThread()
{
    bool ok {};
    SCOPEDTRACE([] () { return "UDP Send Recv thread"; }, [&]{
        return serialization::Serialize(ok);
    });
    IPV4Socket serverSocket(SocketType::Datagram);
    IPV4EndPoint serverAddress(TestPort);

    serverSocket.Bind(serverAddress);

    const std::size_t BufferSize = 10;
    std::uint8_t buffer[BufferSize];
    IPV4EndPoint clientAddress;
    std::size_t bytesReceived = serverSocket.ReceiveFrom(clientAddress, buffer, BufferSize);
    serverSocket.SendTo(clientAddress, buffer, bytesReceived);
    ok = true;

    return ok;
}

TEST_F(IPV4SocketTest, SendReceiveUDPConnected)
{
    IPV4Socket clientSocket(SocketType::Datagram);
    IPV4EndPoint serverAddress(IPV4Address::LocalHost, TestPort);

    BoolReturnThread serverThread(IPV4SocketUDPServerThread);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    bool connected = clientSocket.Connect(serverAddress, 5000);
    EXPECT_TRUE(connected);
    if (connected)
    {
        const std::size_t BufferSize = 10;
        std::uint8_t bufferOut[BufferSize] = { 'H', 'e', 'l', 'l', 'o', 'W', 'o', 'r', 'l', 'd'};
        std::uint8_t bufferIn[BufferSize];
        EXPECT_TRUE(clientSocket.Send(bufferOut, BufferSize, 0));
        std::size_t bytesReceived = clientSocket.Receive(bufferIn, BufferSize, 0);
        EXPECT_EQ(BufferSize, bytesReceived);
        EXPECT_TRUE(std::equal(std::begin(bufferIn), std::end(bufferIn), std::begin(bufferOut)));
    }
    serverThread.WaitForDeath();
    bool accepted = serverThread.GetResult();
    EXPECT_TRUE(accepted);
}

TEST_F(IPV4SocketTest, SendReceiveUDPConnectionless)
{
    IPV4Socket clientSocket(SocketType::Datagram);
    IPV4EndPoint serverAddress(IPV4Address::LocalHost, TestPort);

    BoolReturnThread serverThread(IPV4SocketUDPServerThread);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    const std::size_t BufferSize = 10;
    std::uint8_t bufferOut[BufferSize] = { 'H', 'e', 'l', 'l', 'o', 'W', 'o', 'r', 'l', 'd'};
    std::uint8_t bufferIn[BufferSize];
    clientSocket.SendTo(serverAddress, bufferOut, BufferSize);
    IPV4EndPoint peerAddress;
    std::size_t bytesReceived = clientSocket.ReceiveFrom(peerAddress, bufferIn, BufferSize);
    EXPECT_EQ(BufferSize, bytesReceived);
    EXPECT_EQ(serverAddress, peerAddress);
    EXPECT_TRUE(std::equal(std::begin(bufferIn), std::end(bufferIn), std::begin(bufferOut)));
    serverThread.WaitForDeath();
    bool accepted = serverThread.GetResult();
    EXPECT_TRUE(accepted);
}

} // namespace network