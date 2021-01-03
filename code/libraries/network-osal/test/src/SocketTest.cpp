#include "GoogleTest.h"

#include "network-osal/Socket.h"
#include "osal/Utilities.h"
#include "core/Thread.h"
#include "network-osal/IPV4Address.h"
#include "tracing/ScopedTracing.h"
#include "tracing/Tracing.h"

namespace network {

TEST(SocketTest, Construct)
{
    Socket target;
    EXPECT_EQ(InvalidHandleValue, target.GetHandle());
    EXPECT_FALSE(target.IsOpen());
}

TEST(SocketTest, ConstructAndOpen)
{
    Socket target(SocketFamily::Internet, SocketType::Stream);
    EXPECT_TRUE(target.IsOpen());
}

TEST(SocketTest, ConstructCopy)
{
    Socket target(SocketFamily::InternetV4, SocketType::Datagram);
    auto handle = target.GetHandle();
    Socket newSocket(target);
    EXPECT_EQ(handle, target.GetHandle());
    EXPECT_TRUE(target.IsOpen());
    EXPECT_EQ(handle, newSocket.GetHandle());
    EXPECT_TRUE(newSocket.IsOpen());

    // Do not close twice
    newSocket.SetHandle(InvalidHandleValue);
}

TEST(SocketTest, ConstructMove)
{
    Socket target;
    target.Open(SocketFamily::InternetV4, SocketType::Datagram);
    auto handle = target.GetHandle();

    Socket newSocket(std::move(target));
    EXPECT_EQ(InvalidHandleValue, target.GetHandle());
    EXPECT_FALSE(target.IsOpen());
    EXPECT_EQ(handle, newSocket.GetHandle());
    EXPECT_TRUE(newSocket.IsOpen());
}

TEST(SocketTest, AssignMove)
{
    Socket target;
    target.Open(SocketFamily::InternetV4, SocketType::Datagram);
    auto handle = target.GetHandle();

    Socket newSocket;
    newSocket = std::move(target);
    EXPECT_EQ(InvalidHandleValue, target.GetHandle());
    EXPECT_FALSE(target.IsOpen());
    EXPECT_EQ(handle, newSocket.GetHandle());
    EXPECT_TRUE(newSocket.IsOpen());

    target.SetHandle(InvalidHandleValue);
    newSocket.SetHandle(InvalidHandleValue);
}

TEST(SocketTest, GetSetHandle)
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

TEST(SocketTest, Open)
{
    Socket target;
    target.Open(SocketFamily::Internet, SocketType::Stream);
    EXPECT_TRUE(target.IsOpen());
}

TEST(SocketTest, OpenAndReOpen)
{
    Socket target;
    target.Open(SocketFamily::Internet, SocketType::Stream);
    EXPECT_TRUE(target.IsOpen());
    target.Open(SocketFamily::Internet, SocketType::Datagram);
    EXPECT_TRUE(target.IsOpen());
}

TEST(SocketTest, Close)
{
    Socket target;
    target.Open(SocketFamily::Internet, SocketType::Stream);
    EXPECT_TRUE(target.IsOpen());
    target.Close();
    EXPECT_FALSE(target.IsOpen());
}

TEST(SocketTest, GetOptionWithLevel)
{
    Socket target(SocketFamily::Internet, SocketType::Datagram);
    socklen_t size = static_cast<socklen_t>(sizeof(int));
    int value;
    target.GetSocketOption(SocketOptionLevel::Socket, SocketOption::Broadcast, &value, &size);
    EXPECT_FALSE(value != 0);
}

TEST(SocketTest, SetOptionWithLevel)
{
    Socket target(SocketFamily::Internet, SocketType::Datagram);
    EXPECT_FALSE(target.GetSocketOptionBool(SocketOption::Broadcast));
    socklen_t size = static_cast<socklen_t>(sizeof(int));
    int value = 1;
    int actual;
    target.SetSocketOption(SocketOptionLevel::Socket, SocketOption::Broadcast, &value, size);
    target.GetSocketOption(SocketOptionLevel::Socket, SocketOption::Broadcast, &actual, &size);
    EXPECT_EQ(value, actual);
}

TEST(SocketTest, GetOption)
{
    Socket target;
    target.Open(SocketFamily::Internet, SocketType::Datagram);
    socklen_t size = static_cast<socklen_t>(sizeof(int));
    int value;
    target.GetSocketOption(SocketOption::Broadcast, &value, &size);
    EXPECT_FALSE(value != 0);
}

TEST(SocketTest, SetOption)
{
    Socket target;
    target.Open(SocketFamily::Internet, SocketType::Datagram);
    EXPECT_FALSE(target.GetSocketOptionBool(SocketOption::Broadcast));
    socklen_t size = static_cast<socklen_t>(sizeof(int));
    int value = 1;
    int actual;
    target.SetSocketOption(SocketOption::Broadcast, &value, size);
    target.GetSocketOption(SocketOption::Broadcast, &actual, &size);
    EXPECT_EQ(value, actual);
}

TEST(SocketTest, GetSocketOptionBool)
{
    Socket target;
    target.Open(SocketFamily::Internet, SocketType::Datagram);
    EXPECT_FALSE(target.GetSocketOptionBool(SocketOption::Broadcast));
}

TEST(SocketTest, SetSocketOptionBool)
{
    Socket target;
    target.Open(SocketFamily::Internet, SocketType::Datagram);
    EXPECT_FALSE(target.GetSocketOptionBool(SocketOption::Broadcast));
    target.SetSocketOptionBool(SocketOption::Broadcast, true);
    EXPECT_TRUE(target.GetSocketOptionBool(SocketOption::Broadcast));
}

TEST(SocketTest, GetSocketOptionInt)
{
    Socket target;
    target.Open(SocketFamily::Internet, SocketType::Datagram);
    EXPECT_EQ(0, target.GetSocketOptionInt(SocketOption::Broadcast));
}

TEST(SocketTest, SetSocketOptionInt)
{
    Socket target;
    int enableBroadcast = 1;
    target.Open(SocketFamily::Internet, SocketType::Datagram);
    EXPECT_FALSE(target.GetSocketOptionInt(SocketOption::Broadcast));
    target.SetSocketOptionInt(SocketOption::Broadcast, enableBroadcast);
    EXPECT_EQ(enableBroadcast, target.GetSocketOptionInt(SocketOption::Broadcast));
}

TEST(SocketTest, GetBroadcastOption)
{
    Socket target;
    target.Open(SocketFamily::Internet, SocketType::Datagram);
    EXPECT_FALSE(target.GetBroadcastOption());
}

TEST(SocketTest, SetBroadcastOption)
{
    Socket target;
    target.Open(SocketFamily::Internet, SocketType::Datagram);
    EXPECT_FALSE(target.GetBroadcastOption());
    target.SetBroadcastOption(true);
    EXPECT_TRUE(target.GetBroadcastOption());
}

TEST(SocketTest, GetBlockingMode)
{
    Socket target;
    target.Open(SocketFamily::Internet, SocketType::Stream);
    EXPECT_TRUE(target.GetBlockingMode());
}

TEST(SocketTest, SetBlockingMode)
{
    Socket target;
    target.Open(SocketFamily::Internet, SocketType::Stream);
    EXPECT_TRUE(target.GetBlockingMode());
    target.SetBlockingMode(false);
    EXPECT_FALSE(target.GetBlockingMode());
    target.SetBlockingMode(true);
    EXPECT_TRUE(target.GetBlockingMode());
}

TEST(SocketTest, GetReuseAddress)
{
    Socket target;
    target.Open(SocketFamily::Internet, SocketType::Datagram);
    EXPECT_FALSE(target.GetReuseAddress());
}

TEST(SocketTest, SetReuseAddress)
{
    Socket target;
    target.Open(SocketFamily::Internet, SocketType::Datagram);
    EXPECT_FALSE(target.GetReuseAddress());
    target.SetReuseAddress(true);
    EXPECT_TRUE(target.GetReuseAddress());
    target.SetReuseAddress(false);
    EXPECT_FALSE(target.GetReuseAddress());
}

TEST(SocketTest, GetReceiveTimeout)
{
    Socket target;
    target.Open(SocketFamily::Internet, SocketType::Datagram);
    std::chrono::milliseconds timeout(0);
    EXPECT_EQ(timeout, target.GetReceiveTimeout());
}

TEST(SocketTest, SetReceiveTimeout)
{
    Socket target;
    std::chrono::milliseconds timeout(0);
    std::chrono::milliseconds timeoutNew(1000);
    target.Open(SocketFamily::Internet, SocketType::Datagram);
    EXPECT_EQ(timeout, target.GetReceiveTimeout());
    target.SetReceiveTimeout(timeoutNew);
    EXPECT_EQ(timeoutNew, target.GetReceiveTimeout());
    target.SetReceiveTimeout(timeout);
    EXPECT_EQ(timeout, target.GetReceiveTimeout());
}

TEST(SocketTest, GetSendTimeout)
{
    Socket target;
    target.Open(SocketFamily::Internet, SocketType::Datagram);
    std::chrono::milliseconds timeout(0);
    EXPECT_EQ(timeout, target.GetSendTimeout());
}

TEST(SocketTest, SetSendTimeout)
{
    Socket target;
    std::chrono::milliseconds timeout(0);
    std::chrono::milliseconds timeoutNew(1000);
    target.Open(SocketFamily::Internet, SocketType::Datagram);
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

bool TCPAcceptThread()
{
    bool accepted {};
    SCOPEDTRACE("TCP Accept Send Recv thread", [&]{
        return serialization::Serialize(accepted);
    });
    Socket acceptorSocket(SocketFamily::InternetV4, SocketType::Stream);
    sockaddr_in serverAddress {};
    FillAddress(serverAddress, 22222, IPV4Address::LocalHost.GetUInt32());

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

TEST(SocketTest, ConnectAcceptSendReceiveTCP)
{
    tracing::Tracing::SetTracingFunctions(
        nullptr, 
        [](tracing::TraceCategory category) { return (category != tracing::TraceCategory::FunctionBegin) &&
                                                     (category != tracing::TraceCategory::FunctionEnd) &&
                                                     (category != tracing::TraceCategory::Information); });

    Socket clientSocket(SocketFamily::InternetV4, SocketType::Stream);
    sockaddr_in serverAddress {};
    FillAddress(serverAddress, 22222, IPV4Address::LocalHost.GetUInt32());

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
    SCOPEDTRACE("UDP Send Recv thread", [&]{
        return serialization::Serialize(ok);
    });
    Socket serverSocket(SocketFamily::InternetV4, SocketType::Datagram);
    sockaddr_in serverAddress {};
    FillAddress(serverAddress, 22222, IPV4Address::Any.GetUInt32());

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

TEST(SocketTest, SendReceiveUDPConnected)
{
    tracing::Tracing::SetTracingFunctions(
        nullptr, 
        [](tracing::TraceCategory category) { return (category != tracing::TraceCategory::FunctionBegin) &&
                                                     (category != tracing::TraceCategory::FunctionEnd); });

    Socket clientSocket(SocketFamily::InternetV4, SocketType::Datagram);
    sockaddr_in serverAddress {};
    FillAddress(serverAddress, 22222, IPV4Address::LocalHost.GetUInt32());

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

TEST(SocketTest, SendReceiveUDPConnectionless)
{
    tracing::Tracing::SetTracingFunctions(
        nullptr, 
        [](tracing::TraceCategory category) { return (category != tracing::TraceCategory::FunctionBegin) &&
                                                     (category != tracing::TraceCategory::FunctionEnd); });

    Socket clientSocket(SocketFamily::InternetV4, SocketType::Datagram);
    sockaddr_in serverAddress {};
    FillAddress(serverAddress, 22222, IPV4Address::LocalHost.GetUInt32());

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

// TEST(SocketTest, Ping)
// {
//     tracing::Tracing::SetTracingFunctions(
//         nullptr, 
//         [](tracing::TraceCategory /*category*/) { return true; });

//     Socket clientSocket(SocketFamily::InternetV4, SocketType::Raw);
//     int ttl = 64;
//     clientSocket.SetSocketOptionInt(static_cast<SocketOption>(IPSocketOption::TTL), ttl);

//     sockaddr_in serverAddress {};
//     FillAddress(serverAddress, 22222, IPV4Address::LocalHost.GetUInt32());

//     BoolReturnThread serverThread(UDPServerThread);
//     std::this_thread::sleep_for(std::chrono::milliseconds(1000));
//     const std::size_t BufferSize = 10;
//     std::uint8_t bufferOut[BufferSize] = { 'H', 'e', 'l', 'l', 'o', 'W', 'o', 'r', 'l', 'd'};
//     std::uint8_t bufferIn[BufferSize];
//     clientSocket.SendTo(reinterpret_cast<const sockaddr *>(&serverAddress), sizeof(serverAddress), bufferOut, BufferSize);
//     sockaddr_in peerAddress {};
//     socklen_t peerAddressSize = sizeof(peerAddress);
//     std::size_t bytesReceived = clientSocket.ReceiveFrom(reinterpret_cast<sockaddr *>(&peerAddress), &peerAddressSize, bufferIn, BufferSize);
//     EXPECT_EQ(BufferSize, bytesReceived);
//     EXPECT_TRUE(std::equal(std::begin(bufferIn), std::end(bufferIn), std::begin(bufferOut)));
//     serverThread.WaitForDeath();
//     bool accepted = serverThread.GetResult();
//     EXPECT_TRUE(accepted);
// }

} // namespace network