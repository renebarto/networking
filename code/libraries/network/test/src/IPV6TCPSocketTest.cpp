#include "GoogleTest.h"

#include "network/IPV6TCPSocket.h"

#include "tracing/ScopedTracing.h"
#include "tracing/Tracing.h"
#include "Utility.h"

namespace network {

class IPV6TCPSocketTest : public ::testing::Test
{
public:
    IPV6TCPSocketTest()
    {
        tracing::Tracing::SetTracingFunctions(
            nullptr, 
            [](tracing::TraceCategory /*category*/) { return false; });
    }
};

TEST_F(IPV6TCPSocketTest, ConstructDefault)
{
    IPV6TCPSocket target;
    EXPECT_NE(InvalidHandleValue, target.GetHandle());
    EXPECT_TRUE(target.IsOpen());
    EXPECT_EQ(SocketFamily::InternetV6, target.Family());
    EXPECT_EQ(SocketType::Stream, target.Type());
}

TEST_F(IPV6TCPSocketTest, ConstructCopy)
{
    IPV6TCPSocket target;
    auto handle = target.GetHandle();
    IPV6TCPSocket newSocket(target);
    EXPECT_EQ(handle, target.GetHandle());
    EXPECT_TRUE(target.IsOpen());
    EXPECT_EQ(handle, newSocket.GetHandle());
    EXPECT_TRUE(newSocket.IsOpen());
    EXPECT_EQ(SocketFamily::InternetV6, target.Family());
    EXPECT_EQ(SocketType::Stream, target.Type());
    EXPECT_EQ(SocketFamily::InternetV6, newSocket.Family());
    EXPECT_EQ(SocketType::Stream, newSocket.Type());

    // Do not close twice
    newSocket.SetHandle(InvalidHandleValue);
}

TEST_F(IPV6TCPSocketTest, ConstructMove)
{
    IPV6TCPSocket target;
    auto handle = target.GetHandle();

    IPV6TCPSocket newSocket(std::move(target));
    EXPECT_EQ(InvalidHandleValue, target.GetHandle());
    EXPECT_FALSE(target.IsOpen());
    EXPECT_EQ(handle, newSocket.GetHandle());
    EXPECT_TRUE(newSocket.IsOpen());
    EXPECT_EQ(SocketFamily::Any, target.Family());
    EXPECT_EQ(SocketType::None, target.Type());
    EXPECT_EQ(SocketFamily::InternetV6, newSocket.Family());
    EXPECT_EQ(SocketType::Stream, newSocket.Type());
}

TEST_F(IPV6TCPSocketTest, AssignMove)
{
    IPV6TCPSocket target;
    auto handle = target.GetHandle();

    IPV6TCPSocket newSocket;
    newSocket = std::move(target);
    EXPECT_EQ(InvalidHandleValue, target.GetHandle());
    EXPECT_FALSE(target.IsOpen());
    EXPECT_EQ(handle, newSocket.GetHandle());
    EXPECT_TRUE(newSocket.IsOpen());
    EXPECT_EQ(SocketFamily::Any, target.Family());
    EXPECT_EQ(SocketType::None, target.Type());
    EXPECT_EQ(SocketFamily::InternetV6, newSocket.Family());
    EXPECT_EQ(SocketType::Stream, newSocket.Type());
}

TEST_F(IPV6TCPSocketTest, GetSetHandle)
{
    IPV6TCPSocket target;
    target.Close();

    SocketHandle handle = 1234;
    target.SetHandle(handle);
    EXPECT_EQ(handle, target.GetHandle());
    EXPECT_TRUE(target.IsOpen());

    target.SetHandle(InvalidHandleValue);
    EXPECT_EQ(InvalidHandleValue, target.GetHandle());
    EXPECT_FALSE(target.IsOpen());
}

TEST_F(IPV6TCPSocketTest, Open)
{
    IPV6TCPSocket target;
    target.Close();

    target.Open();
    EXPECT_TRUE(target.IsOpen());
    EXPECT_EQ(SocketFamily::InternetV6, target.Family());
    EXPECT_EQ(SocketType::Stream, target.Type());
}

TEST_F(IPV6TCPSocketTest, Close)
{
    IPV6TCPSocket target;
    EXPECT_TRUE(target.IsOpen());
    target.Close();
    EXPECT_FALSE(target.IsOpen());
    EXPECT_EQ(SocketFamily::Any, target.Family());
    EXPECT_EQ(SocketType::None, target.Type());
}

TEST_F(IPV6TCPSocketTest, GetOptionWithLevel)
{
    IPV6TCPSocket target;
    socklen_t size = static_cast<socklen_t>(sizeof(int));
    int value;
    target.GetSocketOptionWithLevel(SocketOptionLevel::Socket, SocketOption::Linger, &value, &size);
    EXPECT_FALSE(value != 0);
}

TEST_F(IPV6TCPSocketTest, SetOptionWithLevel)
{
    IPV6TCPSocket target;
    EXPECT_FALSE(target.GetSocketOptionBool(SocketOption::Linger));
    socklen_t size = static_cast<socklen_t>(sizeof(int));
    int value = 1;
    int actual;
    target.SetSocketOptionWithLevel(SocketOptionLevel::Socket, SocketOption::Linger, &value, size);
    target.GetSocketOptionWithLevel(SocketOptionLevel::Socket, SocketOption::Linger, &actual, &size);
    EXPECT_EQ(value, actual);
}

TEST_F(IPV6TCPSocketTest, GetOption)
{
    IPV6TCPSocket target;
    socklen_t size = static_cast<socklen_t>(sizeof(int));
    int value;
    target.GetSocketOption(SocketOption::Linger, &value, &size);
    EXPECT_FALSE(value != 0);
}

TEST_F(IPV6TCPSocketTest, SetOption)
{
    IPV6TCPSocket target;
    EXPECT_FALSE(target.GetSocketOptionBool(SocketOption::Linger));
    socklen_t size = static_cast<socklen_t>(sizeof(int));
    int value = 1;
    int actual;
    target.SetSocketOption(SocketOption::Linger, &value, size);
    target.GetSocketOption(SocketOption::Linger, &actual, &size);
    EXPECT_EQ(value, actual);
}

TEST_F(IPV6TCPSocketTest, GetSocketOptionBool)
{
    IPV6TCPSocket target;
    EXPECT_FALSE(target.GetSocketOptionBool(SocketOption::Linger));
}

TEST_F(IPV6TCPSocketTest, SetSocketOptionBool)
{
    IPV6TCPSocket target;
    EXPECT_FALSE(target.GetSocketOptionBool(SocketOption::Linger));
    target.SetSocketOptionBool(SocketOption::Linger, true);
    EXPECT_TRUE(target.GetSocketOptionBool(SocketOption::Linger));
}

TEST_F(IPV6TCPSocketTest, GetSocketOptionInt)
{
    IPV6TCPSocket target;
    EXPECT_EQ(0, target.GetSocketOptionInt(SocketOption::Linger));
}

TEST_F(IPV6TCPSocketTest, SetSocketOptionInt)
{
    int enableLinger = 1;
    IPV6TCPSocket target;
    EXPECT_FALSE(target.GetSocketOptionInt(SocketOption::Linger));
    target.SetSocketOptionInt(SocketOption::Linger, enableLinger);
    EXPECT_EQ(enableLinger, target.GetSocketOptionInt(SocketOption::Linger));
}

TEST_F(IPV6TCPSocketTest, GetBlockingMode)
{
    IPV6TCPSocket target;
    EXPECT_TRUE(target.GetBlockingMode());
}

TEST_F(IPV6TCPSocketTest, SetBlockingMode)
{
    IPV6TCPSocket target;
    EXPECT_TRUE(target.GetBlockingMode());
    target.SetBlockingMode(false);
    EXPECT_FALSE(target.GetBlockingMode());
    target.SetBlockingMode(true);
    EXPECT_TRUE(target.GetBlockingMode());
}

TEST_F(IPV6TCPSocketTest, GetReuseAddress)
{
    IPV6TCPSocket target;
    EXPECT_FALSE(target.GetReuseAddress());
}

TEST_F(IPV6TCPSocketTest, SetReuseAddress)
{
    IPV6TCPSocket target;
    EXPECT_FALSE(target.GetReuseAddress());
    target.SetReuseAddress(true);
    EXPECT_TRUE(target.GetReuseAddress());
    target.SetReuseAddress(false);
    EXPECT_FALSE(target.GetReuseAddress());
}

TEST_F(IPV6TCPSocketTest, GetReceiveTimeout)
{
    IPV6TCPSocket target;
    std::chrono::milliseconds timeout(0);
    EXPECT_EQ(timeout, target.GetReceiveTimeout());
}

TEST_F(IPV6TCPSocketTest, SetReceiveTimeout)
{
    IPV6TCPSocket target;
    std::chrono::milliseconds timeout(0);
    std::chrono::milliseconds timeoutNew(1000);
    EXPECT_EQ(timeout, target.GetReceiveTimeout());
    target.SetReceiveTimeout(timeoutNew);
    EXPECT_EQ(timeoutNew, target.GetReceiveTimeout());
    target.SetReceiveTimeout(timeout);
    EXPECT_EQ(timeout, target.GetReceiveTimeout());
}

TEST_F(IPV6TCPSocketTest, GetSendTimeout)
{
    IPV6TCPSocket target;
    std::chrono::milliseconds timeout(0);
    EXPECT_EQ(timeout, target.GetSendTimeout());
}

TEST_F(IPV6TCPSocketTest, SetSendTimeout)
{
    IPV6TCPSocket target;
    std::chrono::milliseconds timeout(0);
    std::chrono::milliseconds timeoutNew(1000);
    EXPECT_EQ(timeout, target.GetSendTimeout());
    target.SetSendTimeout(timeoutNew);
    EXPECT_EQ(timeoutNew, target.GetSendTimeout());
    target.SetSendTimeout(timeout);
    EXPECT_EQ(timeout, target.GetSendTimeout());
}

bool IPV6TCPSocketTCPAcceptThread()
{
    bool accepted {};
    SCOPEDTRACE([] () { return "TCP Accept Send Recv thread"; }, [&]{
        return serialization::Serialize(accepted);
    });
    IPV6TCPSocket acceptorSocket;

    acceptorSocket.Bind(IPV6EndPoint(TestPort));
    acceptorSocket.Listen(1);
    IPV6TCPSocket newSocket;
    IPV6EndPoint clientAddress;
    accepted = acceptorSocket.Accept(newSocket, clientAddress, 5000);
    const std::size_t BufferSize = 10;
    std::uint8_t buffer[BufferSize];
    std::size_t bytesReceived = newSocket.Receive(buffer, BufferSize, 0);
    newSocket.Send(buffer, bytesReceived, 0);

    return accepted;
}

TEST_F(IPV6TCPSocketTest, ConnectAcceptSendReceiveTCP)
{
    IPV6TCPSocket clientSocket;
    IPV6EndPoint serverAddress(IPV6Address::LocalHost, TestPort);

    BoolReturnThread acceptorThread(IPV6TCPSocketTCPAcceptThread);
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

} // namespace network