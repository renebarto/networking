#include "GoogleTest.h"

#include "network/IPV4UDPSocket.h"

#include "tracing/ScopedTracing.h"
#include "tracing/Tracing.h"
#include "Utility.h"

namespace network {

class IPV4UDPSocketTest : public ::testing::Test
{
public:
    IPV4UDPSocketTest()
    {
        tracing::Tracing::SetTracingFunctions(
            nullptr, 
            [](tracing::TraceCategory /*category*/) { return false; });
    }
};

TEST_F(IPV4UDPSocketTest, ConstructDefault)
{
    IPV4UDPSocket target;
    EXPECT_NE(InvalidHandleValue, target.GetHandle());
    EXPECT_TRUE(target.IsOpen());
    EXPECT_EQ(SocketFamily::InternetV4, target.Family());
    EXPECT_EQ(SocketType::Datagram, target.Type());
}

TEST_F(IPV4UDPSocketTest, ConstructCopy)
{
    IPV4UDPSocket target;
    auto handle = target.GetHandle();
    IPV4UDPSocket newSocket(target);
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

TEST_F(IPV4UDPSocketTest, ConstructMove)
{
    IPV4UDPSocket target;
    auto handle = target.GetHandle();

    IPV4UDPSocket newSocket(std::move(target));
    EXPECT_EQ(InvalidHandleValue, target.GetHandle());
    EXPECT_FALSE(target.IsOpen());
    EXPECT_EQ(handle, newSocket.GetHandle());
    EXPECT_TRUE(newSocket.IsOpen());
    EXPECT_EQ(SocketFamily::Any, target.Family());
    EXPECT_EQ(SocketType::None, target.Type());
    EXPECT_EQ(SocketFamily::InternetV4, newSocket.Family());
    EXPECT_EQ(SocketType::Datagram, newSocket.Type());
}

TEST_F(IPV4UDPSocketTest, AssignMove)
{
    IPV4UDPSocket target;
    auto handle = target.GetHandle();

    IPV4UDPSocket newSocket;
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

TEST_F(IPV4UDPSocketTest, GetSetHandle)
{
    IPV4UDPSocket target;
    target.Close();

    SocketHandle handle = 1234;
    target.SetHandle(handle);
    EXPECT_EQ(handle, target.GetHandle());
    EXPECT_TRUE(target.IsOpen());

    target.SetHandle(InvalidHandleValue);
    EXPECT_EQ(InvalidHandleValue, target.GetHandle());
    EXPECT_FALSE(target.IsOpen());
}

TEST_F(IPV4UDPSocketTest, Open)
{
    IPV4UDPSocket target;
    target.Close();

    target.Open();
    EXPECT_TRUE(target.IsOpen());
    EXPECT_EQ(SocketFamily::InternetV4, target.Family());
    EXPECT_EQ(SocketType::Datagram, target.Type());
}

TEST_F(IPV4UDPSocketTest, Close)
{
    IPV4UDPSocket target;
    EXPECT_TRUE(target.IsOpen());
    target.Close();
    EXPECT_FALSE(target.IsOpen());
    EXPECT_EQ(SocketFamily::Any, target.Family());
    EXPECT_EQ(SocketType::None, target.Type());
}

TEST_F(IPV4UDPSocketTest, GetOptionWithLevel)
{
    IPV4UDPSocket target;
    socklen_t size = static_cast<socklen_t>(sizeof(int));
    int value;
    target.GetSocketOptionWithLevel(SocketOptionLevel::Socket, SocketOption::Broadcast, &value, &size);
    EXPECT_FALSE(value != 0);
}

TEST_F(IPV4UDPSocketTest, SetOptionWithLevel)
{
    IPV4UDPSocket target;
    EXPECT_FALSE(target.GetSocketOptionBool(SocketOption::Broadcast));
    socklen_t size = static_cast<socklen_t>(sizeof(int));
    int value = 1;
    int actual;
    target.SetSocketOptionWithLevel(SocketOptionLevel::Socket, SocketOption::Broadcast, &value, size);
    target.GetSocketOptionWithLevel(SocketOptionLevel::Socket, SocketOption::Broadcast, &actual, &size);
    EXPECT_EQ(value, actual);
}

TEST_F(IPV4UDPSocketTest, GetOption)
{
    IPV4UDPSocket target;
    socklen_t size = static_cast<socklen_t>(sizeof(int));
    int value;
    target.GetSocketOption(SocketOption::Broadcast, &value, &size);
    EXPECT_FALSE(value != 0);
}

TEST_F(IPV4UDPSocketTest, SetOption)
{
    IPV4UDPSocket target;
    EXPECT_FALSE(target.GetSocketOptionBool(SocketOption::Broadcast));
    socklen_t size = static_cast<socklen_t>(sizeof(int));
    int value = 1;
    int actual;
    target.SetSocketOption(SocketOption::Broadcast, &value, size);
    target.GetSocketOption(SocketOption::Broadcast, &actual, &size);
    EXPECT_EQ(value, actual);
}

TEST_F(IPV4UDPSocketTest, GetSocketOptionBool)
{
    IPV4UDPSocket target;
    EXPECT_FALSE(target.GetSocketOptionBool(SocketOption::Broadcast));
}

TEST_F(IPV4UDPSocketTest, SetSocketOptionBool)
{
    IPV4UDPSocket target;
    EXPECT_FALSE(target.GetSocketOptionBool(SocketOption::Broadcast));
    target.SetSocketOptionBool(SocketOption::Broadcast, true);
    EXPECT_TRUE(target.GetSocketOptionBool(SocketOption::Broadcast));
}

TEST_F(IPV4UDPSocketTest, GetSocketOptionInt)
{
    IPV4UDPSocket target;
    EXPECT_EQ(0, target.GetSocketOptionInt(SocketOption::Broadcast));
}

TEST_F(IPV4UDPSocketTest, SetSocketOptionInt)
{
    int enableBroadcast = 1;
    IPV4UDPSocket target;
    EXPECT_FALSE(target.GetSocketOptionInt(SocketOption::Broadcast));
    target.SetSocketOptionInt(SocketOption::Broadcast, enableBroadcast);
    EXPECT_EQ(enableBroadcast, target.GetSocketOptionInt(SocketOption::Broadcast));
}

TEST_F(IPV4UDPSocketTest, GetBroadcastOption)
{
    IPV4UDPSocket target;
    EXPECT_FALSE(target.GetBroadcastOption());
}

TEST_F(IPV4UDPSocketTest, SetBroadcastOption)
{
    IPV4UDPSocket target;
    EXPECT_FALSE(target.GetBroadcastOption());
    target.SetBroadcastOption(true);
    EXPECT_TRUE(target.GetBroadcastOption());
}

TEST_F(IPV4UDPSocketTest, GetBlockingMode)
{
    IPV4UDPSocket target;
    EXPECT_TRUE(target.GetBlockingMode());
}

TEST_F(IPV4UDPSocketTest, SetBlockingMode)
{
    IPV4UDPSocket target;
    EXPECT_TRUE(target.GetBlockingMode());
    target.SetBlockingMode(false);
    EXPECT_FALSE(target.GetBlockingMode());
    target.SetBlockingMode(true);
    EXPECT_TRUE(target.GetBlockingMode());
}

TEST_F(IPV4UDPSocketTest, GetReuseAddress)
{
    IPV4UDPSocket target;
    EXPECT_FALSE(target.GetReuseAddress());
}

TEST_F(IPV4UDPSocketTest, SetReuseAddress)
{
    IPV4UDPSocket target;
    EXPECT_FALSE(target.GetReuseAddress());
    target.SetReuseAddress(true);
    EXPECT_TRUE(target.GetReuseAddress());
    target.SetReuseAddress(false);
    EXPECT_FALSE(target.GetReuseAddress());
}

TEST_F(IPV4UDPSocketTest, GetReceiveTimeout)
{
    IPV4UDPSocket target;
    std::chrono::milliseconds timeout(0);
    EXPECT_EQ(timeout, target.GetReceiveTimeout());
}

TEST_F(IPV4UDPSocketTest, SetReceiveTimeout)
{
    IPV4UDPSocket target;
    std::chrono::milliseconds timeout(0);
    std::chrono::milliseconds timeoutNew(1000);
    EXPECT_EQ(timeout, target.GetReceiveTimeout());
    target.SetReceiveTimeout(timeoutNew);
    EXPECT_EQ(timeoutNew, target.GetReceiveTimeout());
    target.SetReceiveTimeout(timeout);
    EXPECT_EQ(timeout, target.GetReceiveTimeout());
}

TEST_F(IPV4UDPSocketTest, GetSendTimeout)
{
    IPV4UDPSocket target;
    std::chrono::milliseconds timeout(0);
    EXPECT_EQ(timeout, target.GetSendTimeout());
}

TEST_F(IPV4UDPSocketTest, SetSendTimeout)
{
    IPV4UDPSocket target;
    std::chrono::milliseconds timeout(0);
    std::chrono::milliseconds timeoutNew(1000);
    EXPECT_EQ(timeout, target.GetSendTimeout());
    target.SetSendTimeout(timeoutNew);
    EXPECT_EQ(timeoutNew, target.GetSendTimeout());
    target.SetSendTimeout(timeout);
    EXPECT_EQ(timeout, target.GetSendTimeout());
}

bool IPV4UDPSocketUDPServerThread()
{
    bool ok {};
    SCOPEDTRACE([] () { return "UDP Send Recv thread"; }, [&]{
        return serialization::Serialize(ok);
    });
    IPV4UDPSocket serverSocket;
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

TEST_F(IPV4UDPSocketTest, SendReceiveUDPConnected)
{
    IPV4UDPSocket clientSocket;
    IPV4EndPoint serverAddress(IPV4Address::LocalHost, TestPort);

    BoolReturnThread serverThread(IPV4UDPSocketUDPServerThread);
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

TEST_F(IPV4UDPSocketTest, SendReceiveUDPConnectionless)
{
    IPV4UDPSocket clientSocket;
    IPV4EndPoint serverAddress(IPV4Address::LocalHost, TestPort);

    BoolReturnThread serverThread(IPV4UDPSocketUDPServerThread);
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