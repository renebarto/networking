#include "GoogleTest.h"

#include "network/IPV6Socket.h"

#include "tracing/ScopedTracing.h"
#include "tracing/Logging.h"
#include "utility/GenericError.h"
#include "Utility.h"

namespace network {

class IPV6SocketTest : public ::testing::Test
{
public:
    IPV6SocketTest()
    {
        tracing::Tracing::SetTracingFunctions(
            nullptr, 
            [](tracing::TraceCategory /*category*/) { return false; });
    }
};

TEST_F(IPV6SocketTest, ConstructDefault)
{
    IPV6Socket target;
    EXPECT_EQ(InvalidHandleValue, target.GetHandle());
    EXPECT_FALSE(target.IsOpen());
    EXPECT_EQ(SocketFamily::Any, target.Family());
    EXPECT_EQ(SocketType::None, target.Type());
}

TEST_F(IPV6SocketTest, Construct)
{
    IPV6Socket target(SocketType::Datagram);
    EXPECT_NE(InvalidHandleValue, target.GetHandle());
    EXPECT_TRUE(target.IsOpen());
    EXPECT_EQ(SocketFamily::InternetV6, target.Family());
    EXPECT_EQ(SocketType::Datagram, target.Type());
}

TEST_F(IPV6SocketTest, ConstructCopy)
{
    IPV6Socket target(SocketType::Datagram);
    auto handle = target.GetHandle();
    IPV6Socket newSocket(target);
    EXPECT_EQ(handle, target.GetHandle());
    EXPECT_TRUE(target.IsOpen());
    EXPECT_EQ(handle, newSocket.GetHandle());
    EXPECT_TRUE(newSocket.IsOpen());
    EXPECT_EQ(SocketFamily::InternetV6, target.Family());
    EXPECT_EQ(SocketType::Datagram, target.Type());
    EXPECT_EQ(SocketFamily::InternetV6, newSocket.Family());
    EXPECT_EQ(SocketType::Datagram, newSocket.Type());

    // Do not close twice
    newSocket.SetHandle(InvalidHandleValue);
}

TEST_F(IPV6SocketTest, ConstructMove)
{
    IPV6Socket target(SocketType::Datagram);
    auto handle = target.GetHandle();

    IPV6Socket newSocket(std::move(target));
    EXPECT_EQ(InvalidHandleValue, target.GetHandle());
    EXPECT_FALSE(target.IsOpen());
    EXPECT_EQ(handle, newSocket.GetHandle());
    EXPECT_TRUE(newSocket.IsOpen());
    EXPECT_EQ(SocketFamily::Any, target.Family());
    EXPECT_EQ(SocketType::None, target.Type());
    EXPECT_EQ(SocketFamily::InternetV6, newSocket.Family());
    EXPECT_EQ(SocketType::Datagram, newSocket.Type());
}

TEST_F(IPV6SocketTest, AssignMove)
{
    IPV6Socket target(SocketType::Datagram);
    auto handle = target.GetHandle();

    IPV6Socket newSocket;
    newSocket = std::move(target);
    EXPECT_EQ(InvalidHandleValue, target.GetHandle());
    EXPECT_FALSE(target.IsOpen());
    EXPECT_EQ(handle, newSocket.GetHandle());
    EXPECT_TRUE(newSocket.IsOpen());
    EXPECT_EQ(SocketFamily::Any, target.Family());
    EXPECT_EQ(SocketType::None, target.Type());
    EXPECT_EQ(SocketFamily::InternetV6, newSocket.Family());
    EXPECT_EQ(SocketType::Datagram, newSocket.Type());
}

TEST_F(IPV6SocketTest, GetSetHandle)
{
    IPV6Socket target;
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
    IPV6Socket target;
    target.Open(SocketType::Stream);
    EXPECT_TRUE(target.IsOpen());
    EXPECT_EQ(SocketFamily::InternetV6, target.Family());
    EXPECT_EQ(SocketType::Stream, target.Type());
}

TEST_F(IPV6SocketTest, OpenAndReOpen)
{
    IPV6Socket target;
    target.Open(SocketType::Stream);
    EXPECT_TRUE(target.IsOpen());
    EXPECT_EQ(SocketFamily::InternetV6, target.Family());
    EXPECT_EQ(SocketType::Stream, target.Type());
    target.Open(SocketType::Datagram);
    EXPECT_TRUE(target.IsOpen());
    EXPECT_EQ(SocketFamily::InternetV6, target.Family());
    EXPECT_EQ(SocketType::Datagram, target.Type());
}

TEST_F(IPV6SocketTest, Close)
{
    IPV6Socket target;
    target.Open(SocketType::Stream);
    EXPECT_TRUE(target.IsOpen());
    target.Close();
    EXPECT_FALSE(target.IsOpen());
    EXPECT_EQ(SocketFamily::Any, target.Family());
    EXPECT_EQ(SocketType::None, target.Type());
}

TEST_F(IPV6SocketTest, GetOptionWithLevel)
{
    IPV6Socket target(SocketType::Datagram);
    socklen_t size = static_cast<socklen_t>(sizeof(int));
    int value;
    target.GetSocketOptionWithLevel(SocketOptionLevel::Socket, SocketOption::Broadcast, &value, &size);
    EXPECT_FALSE(value != 0);
}

TEST_F(IPV6SocketTest, SetOptionWithLevel)
{
    IPV6Socket target(SocketType::Datagram);
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
    IPV6Socket target(SocketType::Datagram);
    socklen_t size = static_cast<socklen_t>(sizeof(int));
    int value;
    target.GetSocketOption(SocketOption::Broadcast, &value, &size);
    EXPECT_FALSE(value != 0);
}

TEST_F(IPV6SocketTest, SetOption)
{
    IPV6Socket target(SocketType::Datagram);
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
    IPV6Socket target(SocketType::Datagram);
    EXPECT_FALSE(target.GetSocketOptionBool(SocketOption::Broadcast));
}

TEST_F(IPV6SocketTest, SetSocketOptionBool)
{
    IPV6Socket target(SocketType::Datagram);
    EXPECT_FALSE(target.GetSocketOptionBool(SocketOption::Broadcast));
    target.SetSocketOptionBool(SocketOption::Broadcast, true);
    EXPECT_TRUE(target.GetSocketOptionBool(SocketOption::Broadcast));
}

TEST_F(IPV6SocketTest, GetSocketOptionInt)
{
    IPV6Socket target(SocketType::Datagram);
    EXPECT_EQ(0, target.GetSocketOptionInt(SocketOption::Broadcast));
}

TEST_F(IPV6SocketTest, SetSocketOptionInt)
{
    int enableBroadcast = 1;
    IPV6Socket target(SocketType::Datagram);
    EXPECT_FALSE(target.GetSocketOptionInt(SocketOption::Broadcast));
    target.SetSocketOptionInt(SocketOption::Broadcast, enableBroadcast);
    EXPECT_EQ(enableBroadcast, target.GetSocketOptionInt(SocketOption::Broadcast));
}

TEST_F(IPV6SocketTest, GetBroadcastOption)
{
    IPV6Socket target(SocketType::Datagram);
    EXPECT_FALSE(target.GetBroadcastOption());
}

TEST_F(IPV6SocketTest, SetBroadcastOption)
{
    IPV6Socket target(SocketType::Datagram);
    EXPECT_FALSE(target.GetBroadcastOption());
    target.SetBroadcastOption(true);
    EXPECT_TRUE(target.GetBroadcastOption());
}

TEST_F(IPV6SocketTest, GetBlockingMode)
{
    IPV6Socket target(SocketType::Stream);
    EXPECT_TRUE(target.GetBlockingMode());
}

TEST_F(IPV6SocketTest, SetBlockingMode)
{
    IPV6Socket target(SocketType::Stream);
    EXPECT_TRUE(target.GetBlockingMode());
    target.SetBlockingMode(false);
    EXPECT_FALSE(target.GetBlockingMode());
    target.SetBlockingMode(true);
    EXPECT_TRUE(target.GetBlockingMode());
}

TEST_F(IPV6SocketTest, GetReuseAddress)
{
    IPV6Socket target(SocketType::Datagram);
    EXPECT_FALSE(target.GetReuseAddress());
}

TEST_F(IPV6SocketTest, SetReuseAddress)
{
    IPV6Socket target(SocketType::Datagram);
    EXPECT_FALSE(target.GetReuseAddress());
    target.SetReuseAddress(true);
    EXPECT_TRUE(target.GetReuseAddress());
    target.SetReuseAddress(false);
    EXPECT_FALSE(target.GetReuseAddress());
}

TEST_F(IPV6SocketTest, GetReceiveTimeout)
{
    IPV6Socket target(SocketType::Datagram);
    std::chrono::milliseconds timeout(0);
    EXPECT_EQ(timeout, target.GetReceiveTimeout());
}

TEST_F(IPV6SocketTest, SetReceiveTimeout)
{
    IPV6Socket target(SocketType::Datagram);
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
    IPV6Socket target(SocketType::Datagram);
    std::chrono::milliseconds timeout(0);
    EXPECT_EQ(timeout, target.GetSendTimeout());
}

TEST_F(IPV6SocketTest, SetSendTimeout)
{
    IPV6Socket target(SocketType::Datagram);
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
    bool accepted {};
    SCOPEDTRACE([] () { return "TCP Accept Send Recv thread"; }, [&]{
        return serialization::Serialize(accepted);
    });
    IPV6Socket acceptorSocket(SocketType::Stream);

    acceptorSocket.Bind(IPV6EndPoint(TestPort));
    acceptorSocket.Listen(1);
    IPV6Socket newSocket;
    IPV6EndPoint clientAddress;
    accepted = acceptorSocket.Accept(newSocket, clientAddress, 5000);
    const std::size_t BufferSize = 10;
    std::uint8_t buffer[BufferSize];
    std::size_t bytesReceived = newSocket.Receive(buffer, BufferSize, 0);
    newSocket.Send(buffer, bytesReceived, 0);
    // Wait for client to close connection, otherwise we'll end up in TIME_WAIT status
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    return accepted;
}

TEST_F(IPV6SocketTest, ConnectAcceptSendReceiveTCP)
{
    IPV6Socket clientSocket(SocketType::Stream);
    IPV6EndPoint serverAddress(IPV6Address::LocalHost, TestPort);

    BoolReturnThread acceptorThread(IPV6SocketTCPAcceptThread);
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
        // Make sure to close client before server ends, otherwise we'll end up in TIME_WAIT status
        clientSocket.Close();
    }
    acceptorThread.WaitForDeath();
    bool accepted = acceptorThread.GetResult();
    EXPECT_TRUE(accepted);
}

bool IPV6SocketUDPServerThread()
{
    bool ok {};
    try
    {
        SCOPEDTRACE([] () { return "UDP Send Recv thread"; }, [&]{
            return serialization::Serialize(ok);
        });
        IPV6Socket serverSocket(SocketType::Datagram);
        IPV6EndPoint serverAddress(TestPort);

        serverSocket.Bind(serverAddress);

        const std::size_t BufferSize = 10;
        std::uint8_t buffer[BufferSize];
        IPV6EndPoint clientAddress;
        std::size_t bytesReceived = serverSocket.ReceiveFrom(clientAddress, buffer, BufferSize);
        serverSocket.SendTo(clientAddress, buffer, bytesReceived);
        ok = true;
    }
    catch (std::exception & e)
    {
        tracing::Logging::Error(__FILE__, __LINE__, __func__, utility::GenericError(e.what()));
        throw;
    }
    return ok;
}

TEST_F(IPV6SocketTest, SendReceiveUDPConnected)
{
    IPV6Socket clientSocket(SocketType::Datagram);
    IPV6EndPoint serverAddress(IPV6Address::LocalHost, TestPort);

    BoolReturnThread serverThread(IPV6SocketUDPServerThread);
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

TEST_F(IPV6SocketTest, SendReceiveUDPConnectionless)
{
    IPV6Socket clientSocket(SocketType::Datagram);
    IPV6EndPoint serverAddress(IPV6Address::LocalHost, TestPort);

    BoolReturnThread serverThread(IPV6SocketUDPServerThread);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    const std::size_t BufferSize = 10;
    std::uint8_t bufferOut[BufferSize] = { 'H', 'e', 'l', 'l', 'o', 'W', 'o', 'r', 'l', 'd'};
    std::uint8_t bufferIn[BufferSize];
    clientSocket.SendTo(serverAddress, bufferOut, BufferSize);
    IPV6EndPoint peerAddress;
    std::size_t bytesReceived = clientSocket.ReceiveFrom(peerAddress, bufferIn, BufferSize);
    EXPECT_EQ(BufferSize, bytesReceived);
    EXPECT_EQ(serverAddress, peerAddress);
    EXPECT_TRUE(std::equal(std::begin(bufferIn), std::end(bufferIn), std::begin(bufferOut)));
    serverThread.WaitForDeath();
    bool accepted = serverThread.GetResult();
    EXPECT_TRUE(accepted);
}

} // namespace network