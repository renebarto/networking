#include "GoogleTest.h"

#include "network-osal/Socket.h"
#include "osal/Utilities.h"
#include "core/Thread.h"
#include "network-osal/IPV4Address.h"
#include "tracing/ScopedTracing.h"
#include "tracing/Tracing.h"

namespace network {

class SocketTest : public ::testing::Test
{
public:
    SocketTest()
    {
        tracing::Tracing::SetTracingFunctions(
            nullptr, 
            [](tracing::TraceCategory /*category*/) { return false; });
    }
};

TEST_F(SocketTest, Construct)
{
    Socket target;
    EXPECT_EQ(InvalidHandleValue, target.GetHandle());
    EXPECT_FALSE(target.IsOpen());
    EXPECT_EQ(SocketFamily::Any, target.Family());
    EXPECT_EQ(SocketType::None, target.Type());
}

TEST_F(SocketTest, ConstructAndOpen)
{
    Socket target(SocketFamily::InternetV4, SocketType::Stream);
    EXPECT_TRUE(target.IsOpen());
    EXPECT_EQ(SocketFamily::InternetV4, target.Family());
    EXPECT_EQ(SocketType::Stream, target.Type());
}

TEST_F(SocketTest, ConstructCopy)
{
    Socket target(SocketFamily::InternetV4, SocketType::Datagram);
    auto handle = target.GetHandle();
    Socket newSocket(target);
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

TEST_F(SocketTest, ConstructMove)
{
    Socket target(SocketFamily::InternetV4, SocketType::Datagram);
    auto handle = target.GetHandle();

    Socket newSocket(std::move(target));
    EXPECT_EQ(InvalidHandleValue, target.GetHandle());
    EXPECT_FALSE(target.IsOpen());
    EXPECT_EQ(handle, newSocket.GetHandle());
    EXPECT_TRUE(newSocket.IsOpen());
    EXPECT_EQ(SocketFamily::Any, target.Family());
    EXPECT_EQ(SocketType::None, target.Type());
    EXPECT_EQ(SocketFamily::InternetV4, newSocket.Family());
    EXPECT_EQ(SocketType::Datagram, newSocket.Type());
}

TEST_F(SocketTest, AssignMove)
{
    Socket target(SocketFamily::InternetV4, SocketType::Datagram);
    auto handle = target.GetHandle();

    Socket newSocket;
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

TEST_F(SocketTest, GetSetHandle)
{
    Socket target;
    EXPECT_EQ(InvalidHandleValue, target.GetHandle());

    SocketHandle handle = 1234;
    target.SetHandle(handle);
    EXPECT_TRUE(target.IsOpen());
    EXPECT_EQ(handle, target.GetHandle());

    target.SetHandle(InvalidHandleValue);
    EXPECT_EQ(InvalidHandleValue, target.GetHandle());
}

TEST_F(SocketTest, Open)
{
    Socket target(SocketFamily::InternetV4, SocketType::Stream);
    EXPECT_TRUE(target.IsOpen());
    EXPECT_EQ(SocketFamily::InternetV4, target.Family());
    EXPECT_EQ(SocketType::Stream, target.Type());
}

TEST_F(SocketTest, OpenAndReOpen)
{
    Socket target(SocketFamily::InternetV4, SocketType::Stream);
    EXPECT_TRUE(target.IsOpen());
    EXPECT_EQ(SocketFamily::InternetV4, target.Family());
    EXPECT_EQ(SocketType::Stream, target.Type());
    target.Open(SocketFamily::InternetV4, SocketType::Datagram);
    EXPECT_TRUE(target.IsOpen());
    EXPECT_EQ(SocketFamily::InternetV4, target.Family());
    EXPECT_EQ(SocketType::Datagram, target.Type());
}

TEST_F(SocketTest, Close)
{
    Socket target(SocketFamily::InternetV4, SocketType::Stream);
    EXPECT_TRUE(target.IsOpen());
    target.Close();
    EXPECT_FALSE(target.IsOpen());
    EXPECT_EQ(SocketFamily::Any, target.Family());
    EXPECT_EQ(SocketType::None, target.Type());
}

TEST_F(SocketTest, GetOptionWithLevel)
{
    Socket target(SocketFamily::InternetV4, SocketType::Datagram);
    socklen_t size = static_cast<socklen_t>(sizeof(int));
    int value;
    target.GetSocketOptionWithLevel(SocketOptionLevel::Socket, SocketOption::Broadcast, &value, &size);
    EXPECT_FALSE(value != 0);
}

TEST_F(SocketTest, SetOptionWithLevel)
{
    Socket target(SocketFamily::InternetV4, SocketType::Datagram);
    EXPECT_FALSE(target.GetSocketOptionBool(SocketOption::Broadcast));
    socklen_t size = static_cast<socklen_t>(sizeof(int));
    int value = 1;
    int actual;
    target.SetSocketOptionWithLevel(SocketOptionLevel::Socket, SocketOption::Broadcast, &value, size);
    target.GetSocketOptionWithLevel(SocketOptionLevel::Socket, SocketOption::Broadcast, &actual, &size);
    EXPECT_EQ(value, actual);
}

TEST_F(SocketTest, GetOption)
{
    Socket target(SocketFamily::InternetV4, SocketType::Datagram);
    socklen_t size = static_cast<socklen_t>(sizeof(int));
    int value;
    target.GetSocketOption(SocketOption::Broadcast, &value, &size);
    EXPECT_FALSE(value != 0);
}

TEST_F(SocketTest, SetOption)
{
    Socket target(SocketFamily::InternetV4, SocketType::Datagram);
    EXPECT_FALSE(target.GetSocketOptionBool(SocketOption::Broadcast));
    socklen_t size = static_cast<socklen_t>(sizeof(int));
    int value = 1;
    int actual;
    target.SetSocketOption(SocketOption::Broadcast, &value, size);
    target.GetSocketOption(SocketOption::Broadcast, &actual, &size);
    EXPECT_EQ(value, actual);
}

TEST_F(SocketTest, GetSocketOptionBool)
{
    Socket target(SocketFamily::InternetV4, SocketType::Datagram);
    EXPECT_FALSE(target.GetSocketOptionBool(SocketOption::Broadcast));
}

TEST_F(SocketTest, SetSocketOptionBool)
{
    Socket target(SocketFamily::InternetV4, SocketType::Datagram);
    EXPECT_FALSE(target.GetSocketOptionBool(SocketOption::Broadcast));
    target.SetSocketOptionBool(SocketOption::Broadcast, true);
    EXPECT_TRUE(target.GetSocketOptionBool(SocketOption::Broadcast));
}

TEST_F(SocketTest, GetSocketOptionInt)
{
    Socket target(SocketFamily::InternetV4, SocketType::Datagram);
    EXPECT_EQ(0, target.GetSocketOptionInt(SocketOption::Broadcast));
}

TEST_F(SocketTest, SetSocketOptionInt)
{
    int enableBroadcast = 1;
    Socket target(SocketFamily::InternetV4, SocketType::Datagram);
    EXPECT_FALSE(target.GetSocketOptionInt(SocketOption::Broadcast));
    target.SetSocketOptionInt(SocketOption::Broadcast, enableBroadcast);
    EXPECT_EQ(enableBroadcast, target.GetSocketOptionInt(SocketOption::Broadcast));
}

TEST_F(SocketTest, GetBroadcastOption)
{
    Socket target(SocketFamily::InternetV4, SocketType::Datagram);
    EXPECT_FALSE(target.GetBroadcastOption());
}

TEST_F(SocketTest, SetBroadcastOption)
{
    Socket target(SocketFamily::InternetV4, SocketType::Datagram);
    EXPECT_FALSE(target.GetBroadcastOption());
    target.SetBroadcastOption(true);
    EXPECT_TRUE(target.GetBroadcastOption());
}

TEST_F(SocketTest, GetBlockingMode)
{
    Socket target(SocketFamily::InternetV4, SocketType::Stream);
    EXPECT_TRUE(target.GetBlockingMode());
}

TEST_F(SocketTest, SetBlockingMode)
{
    Socket target(SocketFamily::InternetV4, SocketType::Stream);
    EXPECT_TRUE(target.GetBlockingMode());
    target.SetBlockingMode(false);
    EXPECT_FALSE(target.GetBlockingMode());
    target.SetBlockingMode(true);
    EXPECT_TRUE(target.GetBlockingMode());
}

TEST_F(SocketTest, GetReuseAddress)
{
    Socket target(SocketFamily::InternetV4, SocketType::Datagram);
    EXPECT_FALSE(target.GetReuseAddress());
}

TEST_F(SocketTest, SetReuseAddress)
{
    Socket target(SocketFamily::InternetV4, SocketType::Datagram);
    EXPECT_FALSE(target.GetReuseAddress());
    target.SetReuseAddress(true);
    EXPECT_TRUE(target.GetReuseAddress());
    target.SetReuseAddress(false);
    EXPECT_FALSE(target.GetReuseAddress());
}

TEST_F(SocketTest, GetReceiveTimeout)
{
    Socket target(SocketFamily::InternetV4, SocketType::Datagram);
    std::chrono::milliseconds timeout(0);
    EXPECT_EQ(timeout, target.GetReceiveTimeout());
}

TEST_F(SocketTest, SetReceiveTimeout)
{
    Socket target(SocketFamily::InternetV4, SocketType::Datagram);
    std::chrono::milliseconds timeout(0);
    std::chrono::milliseconds timeoutNew(1000);
    EXPECT_EQ(timeout, target.GetReceiveTimeout());
    target.SetReceiveTimeout(timeoutNew);
    EXPECT_EQ(timeoutNew, target.GetReceiveTimeout());
    target.SetReceiveTimeout(timeout);
    EXPECT_EQ(timeout, target.GetReceiveTimeout());
}

TEST_F(SocketTest, GetSendTimeout)
{
    Socket target(SocketFamily::InternetV4, SocketType::Datagram);
    std::chrono::milliseconds timeout(0);
    EXPECT_EQ(timeout, target.GetSendTimeout());
}

TEST_F(SocketTest, SetSendTimeout)
{
    Socket target(SocketFamily::InternetV4, SocketType::Datagram);
    std::chrono::milliseconds timeout(0);
    std::chrono::milliseconds timeoutNew(1000);
    EXPECT_EQ(timeout, target.GetSendTimeout());
    target.SetSendTimeout(timeoutNew);
    EXPECT_EQ(timeoutNew, target.GetSendTimeout());
    target.SetSendTimeout(timeout);
    EXPECT_EQ(timeout, target.GetSendTimeout());
}

class BoolReturnThread
    : public core::Thread
{
private:
    std::function<bool ()> m_threadFunc;
    bool m_result;

public:
    BoolReturnThread(std::function<bool ()> threadFunc)
        : Thread()
        , m_threadFunc(threadFunc)
        , m_result()
    {
        Create(std::bind(&BoolReturnThread::ThreadFunc, this));
    }

    bool GetResult() { return m_result; }
    void SetResult(bool value) { m_result = value; }

    void ThreadFunc()
    {
        SetResult(m_threadFunc());
    }
};

void FillAddress(sockaddr_in & address, std::uint16_t port, std::uint32_t ipAddress)
{
    address.sin_family = AF_INET;
    address.sin_port = osal::SwapBytes(port);
    address.sin_addr.s_addr = ipAddress;
}

static const std::uint16_t TestPort = 22222;

bool TCPAcceptThread()
{
    bool accepted {};
    SCOPEDTRACE([] () { return "TCP Accept Send Recv thread"; }, [&]{
        return serialization::Serialize(accepted);
    });
    Socket acceptorSocket(SocketFamily::InternetV4, SocketType::Stream);
    sockaddr_in serverAddress {};
    FillAddress(serverAddress, TestPort, IPV4Address::LocalHost.GetUInt32());

    acceptorSocket.Bind(reinterpret_cast<sockaddr *>(&serverAddress), sizeof(serverAddress));
    acceptorSocket.Listen(1);
    Socket newSocket;
    sockaddr_in clientAddress {};
    socklen_t clientAddressSize = sizeof(clientAddress);
    accepted = acceptorSocket.Accept(newSocket, reinterpret_cast<sockaddr *>(&clientAddress), &clientAddressSize, 5000);
    const std::size_t BufferSize = 10;
    std::uint8_t buffer[BufferSize];
    std::size_t bytesReceived = newSocket.Receive(buffer, BufferSize, 0);
    newSocket.Send(buffer, bytesReceived, 0);

    return accepted;
}

TEST_F(SocketTest, ConnectAcceptSendReceiveTCP)
{
    Socket clientSocket(SocketFamily::InternetV4, SocketType::Stream);
    sockaddr_in serverAddress {};
    FillAddress(serverAddress, TestPort, IPV4Address::LocalHost.GetUInt32());

    BoolReturnThread acceptorThread(TCPAcceptThread);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    bool connected = clientSocket.Connect(reinterpret_cast<sockaddr *>(&serverAddress), sizeof(serverAddress), 5000);
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

bool UDPServerThread()
{
    bool ok {};
    SCOPEDTRACE([] () { return "UDP Send Recv thread"; }, [&]{
        return serialization::Serialize(ok);
    });
    Socket serverSocket(SocketFamily::InternetV4, SocketType::Datagram);
    sockaddr_in serverAddress {};
    FillAddress(serverAddress, TestPort, IPV4Address::Any.GetUInt32());

    serverSocket.Bind(reinterpret_cast<sockaddr *>(&serverAddress), sizeof(serverAddress));

    const std::size_t BufferSize = 10;
    std::uint8_t buffer[BufferSize];
    sockaddr_in clientAddress {};
    socklen_t clientAddressSize = sizeof(clientAddress);
    std::size_t bytesReceived = serverSocket.ReceiveFrom(reinterpret_cast<sockaddr *>(&clientAddress), &clientAddressSize, buffer, BufferSize);
    serverSocket.SendTo(reinterpret_cast<const sockaddr *>(&clientAddress), clientAddressSize, buffer, bytesReceived);
    ok = true;

    return ok;
}

TEST_F(SocketTest, SendReceiveUDPConnected)
{
    Socket clientSocket(SocketFamily::InternetV4, SocketType::Datagram);
    sockaddr_in serverAddress {};
    FillAddress(serverAddress, TestPort, IPV4Address::LocalHost.GetUInt32());

    BoolReturnThread serverThread(UDPServerThread);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    bool connected = clientSocket.Connect(reinterpret_cast<sockaddr *>(&serverAddress), sizeof(serverAddress), 5000);
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

TEST_F(SocketTest, SendReceiveUDPConnectionless)
{
    Socket clientSocket(SocketFamily::InternetV4, SocketType::Datagram);
    sockaddr_in serverAddress {};
    FillAddress(serverAddress, TestPort, IPV4Address::LocalHost.GetUInt32());

    BoolReturnThread serverThread(UDPServerThread);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    const std::size_t BufferSize = 10;
    std::uint8_t bufferOut[BufferSize] = { 'H', 'e', 'l', 'l', 'o', 'W', 'o', 'r', 'l', 'd'};
    std::uint8_t bufferIn[BufferSize];
    clientSocket.SendTo(reinterpret_cast<const sockaddr *>(&serverAddress), sizeof(serverAddress), bufferOut, BufferSize);
    sockaddr_in peerAddress {};
    socklen_t peerAddressSize = sizeof(peerAddress);
    std::size_t bytesReceived = clientSocket.ReceiveFrom(reinterpret_cast<sockaddr *>(&peerAddress), &peerAddressSize, bufferIn, BufferSize);
    EXPECT_EQ(BufferSize, bytesReceived);
    EXPECT_TRUE(std::equal(std::begin(bufferIn), std::end(bufferIn), std::begin(bufferOut)));
    serverThread.WaitForDeath();
    bool accepted = serverThread.GetResult();
    EXPECT_TRUE(accepted);
}

TEST_F(SocketTest, SerializeSocket)
{
    Socket target;

    EXPECT_EQ("-1", serialization::Serialize(target, 0));

    SocketHandle handle = 1234;
    target.SetHandle(handle);
    EXPECT_EQ("1234", serialization::Serialize(target, 0));

    target.SetHandle(InvalidHandleValue);
}

TEST_F(SocketTest, SerializeSockAddrSize)
{
    sockaddr_in address {};
    address.sin_family = AF_INET;

    EXPECT_EQ("addressFamily=2, size=16", serialization::Serialize(reinterpret_cast<sockaddr *>(&address), sizeof(address)));
}

TEST_F(SocketTest, SerializeSockAddrSizePtr)
{
    sockaddr_in address {};
    address.sin_family = AF_INET;
    socklen_t addressSize = sizeof(address);

    EXPECT_EQ("addressFamily=2, size=16", serialization::Serialize(reinterpret_cast<sockaddr *>(&address), &addressSize));
}

} // namespace network