#include "GoogleTest.h"

#include "network-osal/Socket.h"

#include "gmock/gmock.h"

#include "network-osal/SocketAPIMock.h"
#include "osal/Utilities.h"
#include "core/Thread.h"
#include "network-osal/IPV4Address.h"
#include "tracing/ScopedTracing.h"
#include "tracing/Tracing.h"

namespace network {
namespace testing {

using ::testing::DoAll;
using ::testing::SetArgPointee;

class SocketTest : public ::testing::Test
{
public:
    tracing::CategorySet<tracing::TraceCategory> m_savedTraceFilter;

    SocketTest()
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

TEST_F(SocketTest, Construct)
{
    testing::SocketAPIMock api;

    EXPECT_CALL(api, Open(_, _, _)).Times(0);

    Socket target(api);

    EXPECT_EQ(InvalidHandleValue, target.GetHandle());
    EXPECT_FALSE(target.IsOpen());
    EXPECT_EQ(SocketFamily::Any, target.Family());
    EXPECT_EQ(SocketType::None, target.Type());
}

TEST_F(SocketTest, ConstructAndOpen)
{
    // ::testing::NiceMock<testing::SocketAPIMock> api;
    testing::SocketAPIMock api;

    EXPECT_CALL(api, Open(SocketFamily::InternetV4, SocketType::Stream, _)).Times(1);
    EXPECT_CALL(api, Close(_)).Times(1);

    Socket target(api, SocketFamily::InternetV4, SocketType::Stream);

    EXPECT_TRUE(target.IsOpen());
    EXPECT_EQ(SocketFamily::InternetV4, target.Family());
    EXPECT_EQ(SocketType::Stream, target.Type());
}

TEST_F(SocketTest, ConstructCopy)
{
    testing::SocketAPIMock api;

    EXPECT_CALL(api, Open(SocketFamily::InternetV4, SocketType::Datagram, _)).Times(1);
    EXPECT_CALL(api, Close(_)).Times(1);

    Socket target(api, SocketFamily::InternetV4, SocketType::Datagram);
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
    testing::SocketAPIMock api;

    EXPECT_CALL(api, Open(SocketFamily::InternetV4, SocketType::Datagram, _)).Times(1);
    EXPECT_CALL(api, Close(_)).Times(1);

    Socket target(api, SocketFamily::InternetV4, SocketType::Datagram);
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
    testing::SocketAPIMock api;

    EXPECT_CALL(api, Open(SocketFamily::InternetV4, SocketType::Datagram, _)).Times(1);
    EXPECT_CALL(api, Close(_)).Times(1);

    Socket target(api, SocketFamily::InternetV4, SocketType::Datagram);
    auto handle = target.GetHandle();

    Socket newSocket(api);
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
    testing::SocketAPIMock api;

    EXPECT_CALL(api, Open(SocketFamily::InternetV4, SocketType::Stream, _)).Times(0);
    EXPECT_CALL(api, Close(_)).Times(0);

    Socket target(api);
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
    testing::SocketAPIMock api;

    EXPECT_CALL(api, Open(SocketFamily::InternetV4, SocketType::Stream, _)).Times(1);
    EXPECT_CALL(api, Close(_)).Times(1);

    Socket target(api, SocketFamily::InternetV4, SocketType::Stream);
    EXPECT_TRUE(target.IsOpen());
    EXPECT_EQ(SocketFamily::InternetV4, target.Family());
    EXPECT_EQ(SocketType::Stream, target.Type());
}

TEST_F(SocketTest, OpenAndReOpen)
{
    testing::SocketAPIMock api;

    EXPECT_CALL(api, Open(SocketFamily::InternetV4, SocketType::Stream, _)).Times(1);
    EXPECT_CALL(api, Open(SocketFamily::InternetV4, SocketType::Datagram, _)).Times(1);
    EXPECT_CALL(api, Close(_)).Times(2);

    Socket target(api, SocketFamily::InternetV4, SocketType::Stream);
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
    testing::SocketAPIMock api;

    EXPECT_CALL(api, Open(SocketFamily::InternetV4, SocketType::Stream, _)).Times(1);
    EXPECT_CALL(api, Close(_)).Times(1);

    Socket target(api, SocketFamily::InternetV4, SocketType::Stream);
    EXPECT_TRUE(target.IsOpen());
    target.Close();
    EXPECT_FALSE(target.IsOpen());
    EXPECT_EQ(SocketFamily::Any, target.Family());
    EXPECT_EQ(SocketType::None, target.Type());

    target.Close();
    EXPECT_FALSE(target.IsOpen());
    EXPECT_EQ(SocketFamily::Any, target.Family());
    EXPECT_EQ(SocketType::None, target.Type());
}

#if defined(PLATFORM_WINDOWS)
#pragma warning(disable: 4100)
#endif
ACTION_P(SetArg3ToMCValue, value) { *reinterpret_cast<int *>(arg3) = value; return 0; }
#if defined(PLATFORM_WINDOWS)
#pragma warning(default: 4100)
#endif

TEST_F(SocketTest, GetOptionWithLevel)
{
    testing::SocketAPIMock api;
    socklen_t size = static_cast<socklen_t>(sizeof(int));
    int value;

    EXPECT_CALL(api, Open(SocketFamily::InternetV4, SocketType::Datagram, _)).Times(1);
    EXPECT_CALL(api, Close(_)).Times(1);
    EXPECT_CALL(api, GetSocketOption(_, SocketOptionLevel::Socket, SocketOption::Broadcast, &value, &size)).WillOnce(SetArg3ToMCValue(0));

    Socket target(api, SocketFamily::InternetV4, SocketType::Datagram);
    target.GetSocketOptionWithLevel(SocketOptionLevel::Socket, SocketOption::Broadcast, &value, &size);
    EXPECT_FALSE(value != 0);
}

TEST_F(SocketTest, SetOptionWithLevel)
{
    testing::SocketAPIMock api;
    socklen_t size = static_cast<socklen_t>(sizeof(int));
    int value = 1;
    int actual;

    EXPECT_CALL(api, Open(SocketFamily::InternetV4, SocketType::Datagram, _)).Times(1);
    EXPECT_CALL(api, Close(_)).Times(1);
    EXPECT_CALL(api, GetSocketOption(_, SocketOptionLevel::Socket, SocketOption::Broadcast, _, _)).WillOnce(SetArg3ToMCValue(0)).WillOnce(SetArg3ToMCValue(1));
    EXPECT_CALL(api, SetSocketOption(_, SocketOptionLevel::Socket, SocketOption::Broadcast, &value, size)).Times(1);

    Socket target(api, SocketFamily::InternetV4, SocketType::Datagram);
    EXPECT_FALSE(target.GetSocketOptionBool(SocketOption::Broadcast));
    target.SetSocketOptionWithLevel(SocketOptionLevel::Socket, SocketOption::Broadcast, &value, size);
    target.GetSocketOptionWithLevel(SocketOptionLevel::Socket, SocketOption::Broadcast, &actual, &size);
    EXPECT_EQ(value, actual);
}

TEST_F(SocketTest, GetOption)
{
    testing::SocketAPIMock api;
    socklen_t size = static_cast<socklen_t>(sizeof(int));
    int value;

    EXPECT_CALL(api, Open(SocketFamily::InternetV4, SocketType::Datagram, _)).Times(1);
    EXPECT_CALL(api, Close(_)).Times(1);
    EXPECT_CALL(api, GetSocketOption(_, SocketOptionLevel::Socket, SocketOption::Broadcast, &value, &size)).WillOnce(SetArg3ToMCValue(0));

    Socket target(api, SocketFamily::InternetV4, SocketType::Datagram);
    target.GetSocketOption(SocketOption::Broadcast, &value, &size);
    EXPECT_FALSE(value != 0);
}

TEST_F(SocketTest, SetOption)
{
    testing::SocketAPIMock api;
    socklen_t size = static_cast<socklen_t>(sizeof(int));
    int value = 1;
    int actual;

    EXPECT_CALL(api, Open(SocketFamily::InternetV4, SocketType::Datagram, _)).Times(1);
    EXPECT_CALL(api, Close(_)).Times(1);
    EXPECT_CALL(api, GetSocketOption(_, SocketOptionLevel::Socket, SocketOption::Broadcast, _, _)).WillOnce(SetArg3ToMCValue(0)).WillOnce(SetArg3ToMCValue(1));
    EXPECT_CALL(api, SetSocketOption(_, SocketOptionLevel::Socket, SocketOption::Broadcast, &value, size)).Times(1);

    Socket target(api, SocketFamily::InternetV4, SocketType::Datagram);
    EXPECT_FALSE(target.GetSocketOptionBool(SocketOption::Broadcast));
    target.SetSocketOption(SocketOption::Broadcast, &value, size);
    target.GetSocketOption(SocketOption::Broadcast, &actual, &size);
    EXPECT_EQ(value, actual);
}

TEST_F(SocketTest, GetSocketOptionBool)
{
    testing::SocketAPIMock api;

    EXPECT_CALL(api, Open(SocketFamily::InternetV4, SocketType::Datagram, _)).Times(1);
    EXPECT_CALL(api, Close(_)).Times(1);
    EXPECT_CALL(api, GetSocketOption(_, SocketOptionLevel::Socket, SocketOption::Broadcast, _, _)).WillOnce(SetArg3ToMCValue(0));

    Socket target(api, SocketFamily::InternetV4, SocketType::Datagram);
    EXPECT_FALSE(target.GetSocketOptionBool(SocketOption::Broadcast));
}

TEST_F(SocketTest, SetSocketOptionBool)
{
    testing::SocketAPIMock api;

    EXPECT_CALL(api, Open(SocketFamily::InternetV4, SocketType::Datagram, _)).Times(1);
    EXPECT_CALL(api, Close(_)).Times(1);
    EXPECT_CALL(api, GetSocketOption(_, SocketOptionLevel::Socket, SocketOption::Broadcast, _, _)).WillOnce(SetArg3ToMCValue(0)).WillOnce(SetArg3ToMCValue(1));
    EXPECT_CALL(api, SetSocketOption(_, SocketOptionLevel::Socket, SocketOption::Broadcast, _, _)).Times(1);

    Socket target(api, SocketFamily::InternetV4, SocketType::Datagram);
    EXPECT_FALSE(target.GetSocketOptionBool(SocketOption::Broadcast));
    target.SetSocketOptionBool(SocketOption::Broadcast, true);
    EXPECT_TRUE(target.GetSocketOptionBool(SocketOption::Broadcast));
}

TEST_F(SocketTest, GetSocketOptionInt)
{
    testing::SocketAPIMock api;

    EXPECT_CALL(api, Open(SocketFamily::InternetV4, SocketType::Datagram, _)).Times(1);
    EXPECT_CALL(api, Close(_)).Times(1);
    EXPECT_CALL(api, GetSocketOption(_, SocketOptionLevel::Socket, SocketOption::Broadcast, _, _)).WillOnce(SetArg3ToMCValue(0));

    Socket target(api, SocketFamily::InternetV4, SocketType::Datagram);
    EXPECT_EQ(0, target.GetSocketOptionInt(SocketOption::Broadcast));
}

TEST_F(SocketTest, SetSocketOptionInt)
{
    int enableBroadcast = 1;
    testing::SocketAPIMock api;

    EXPECT_CALL(api, Open(SocketFamily::InternetV4, SocketType::Datagram, _)).Times(1);
    EXPECT_CALL(api, Close(_)).Times(1);
    EXPECT_CALL(api, GetSocketOption(_, SocketOptionLevel::Socket, SocketOption::Broadcast, _, _)).WillOnce(SetArg3ToMCValue(0)).WillOnce(SetArg3ToMCValue(1));
    EXPECT_CALL(api, SetSocketOption(_, SocketOptionLevel::Socket, SocketOption::Broadcast, _, _)).Times(1);

    Socket target(api, SocketFamily::InternetV4, SocketType::Datagram);
    EXPECT_FALSE(target.GetSocketOptionInt(SocketOption::Broadcast));
    target.SetSocketOptionInt(SocketOption::Broadcast, enableBroadcast);
    EXPECT_EQ(enableBroadcast, target.GetSocketOptionInt(SocketOption::Broadcast));
}

TEST_F(SocketTest, GetBroadcastOption)
{
    testing::SocketAPIMock api;

    EXPECT_CALL(api, Open(SocketFamily::InternetV4, SocketType::Datagram, _)).Times(1);
    EXPECT_CALL(api, Close(_)).Times(1);
    EXPECT_CALL(api, GetSocketOption(_, SocketOptionLevel::Socket, SocketOption::Broadcast, _, _)).WillOnce(SetArg3ToMCValue(0));

    Socket target(api, SocketFamily::InternetV4, SocketType::Datagram);
    EXPECT_FALSE(target.GetBroadcastOption());
}

TEST_F(SocketTest, SetBroadcastOption)
{
    testing::SocketAPIMock api;

    EXPECT_CALL(api, Open(SocketFamily::InternetV4, SocketType::Datagram, _)).Times(1);
    EXPECT_CALL(api, Close(_)).Times(1);
    EXPECT_CALL(api, GetSocketOption(_, SocketOptionLevel::Socket, SocketOption::Broadcast, _, _)).WillOnce(SetArg3ToMCValue(0)).WillOnce(SetArg3ToMCValue(1));
    EXPECT_CALL(api, SetSocketOption(_, SocketOptionLevel::Socket, SocketOption::Broadcast, _, _)).Times(1);

    Socket target(api, SocketFamily::InternetV4, SocketType::Datagram);
    EXPECT_FALSE(target.GetBroadcastOption());
    target.SetBroadcastOption(true);
    EXPECT_TRUE(target.GetBroadcastOption());
}

#if defined(PLATFORM_WINDOWS)
#pragma warning(disable: 4100)
#endif
ACTION_P(SetArg1ToMCValue, value) { arg1 = value; return 0; }
#if defined(PLATFORM_WINDOWS)
#pragma warning(default: 4100)
#endif

TEST_F(SocketTest, GetBlockingMode)
{
    testing::SocketAPIMock api;

    EXPECT_CALL(api, Open(SocketFamily::InternetV4, SocketType::Stream, _)).Times(1);
    EXPECT_CALL(api, Close(_)).Times(1);
#if defined(PLATFORM_LINUX)
    EXPECT_CALL(api, GetBlockingMode(_, _)).WillOnce(SetArg1ToMCValue(true));
#endif

    Socket target(api, SocketFamily::InternetV4, SocketType::Stream);
    EXPECT_TRUE(target.GetBlockingMode());
}

TEST_F(SocketTest, SetBlockingMode)
{
    testing::SocketAPIMock api;

    EXPECT_CALL(api, Open(SocketFamily::InternetV4, SocketType::Stream, _)).Times(1);
    EXPECT_CALL(api, Close(_)).Times(1);
    EXPECT_CALL(api, SetBlockingMode(_, _)).Times(2);
#if defined(PLATFORM_LINUX)
    EXPECT_CALL(api, GetBlockingMode(_, _)).WillOnce(SetArg1ToMCValue(true)).WillOnce(SetArg1ToMCValue(false)).WillOnce(SetArg1ToMCValue(true));
#endif

    Socket target(api, SocketFamily::InternetV4, SocketType::Stream);
    EXPECT_TRUE(target.GetBlockingMode());
    target.SetBlockingMode(false);
    EXPECT_FALSE(target.GetBlockingMode());
    target.SetBlockingMode(true);
    EXPECT_TRUE(target.GetBlockingMode());
}

TEST_F(SocketTest, GetReuseAddress)
{
    testing::SocketAPIMock api;

    EXPECT_CALL(api, Open(SocketFamily::InternetV4, SocketType::Datagram, _)).Times(1);
    EXPECT_CALL(api, Close(_)).Times(1);
    EXPECT_CALL(api, GetSocketOption(_, SocketOptionLevel::Socket, SocketOption::ReuseAddress, _, _)).WillOnce(SetArg3ToMCValue(0));

    Socket target(api, SocketFamily::InternetV4, SocketType::Datagram);
    EXPECT_FALSE(target.GetReuseAddress());
}

TEST_F(SocketTest, SetReuseAddress)
{
    testing::SocketAPIMock api;

    EXPECT_CALL(api, Open(SocketFamily::InternetV4, SocketType::Datagram, _)).Times(1);
    EXPECT_CALL(api, Close(_)).Times(1);
    EXPECT_CALL(api, GetSocketOption(_, SocketOptionLevel::Socket, SocketOption::ReuseAddress, _, _)).WillOnce(SetArg3ToMCValue(0)).WillOnce(SetArg3ToMCValue(1)).WillOnce(SetArg3ToMCValue(0));
    EXPECT_CALL(api, SetSocketOption(_, SocketOptionLevel::Socket, SocketOption::ReuseAddress, _, _)).Times(2);

    Socket target(api, SocketFamily::InternetV4, SocketType::Datagram);
    EXPECT_FALSE(target.GetReuseAddress());
    target.SetReuseAddress(true);
    EXPECT_TRUE(target.GetReuseAddress());
    target.SetReuseAddress(false);
    EXPECT_FALSE(target.GetReuseAddress());
}

TEST_F(SocketTest, GetReceiveTimeout)
{
    testing::SocketAPIMock api;

    EXPECT_CALL(api, Open(SocketFamily::InternetV4, SocketType::Datagram, _)).Times(1);
    EXPECT_CALL(api, Close(_)).Times(1);
    EXPECT_CALL(api, GetSocketOption(_, SocketOptionLevel::Socket, SocketOption::ReceiveTimeout, _, _)).WillOnce(SetArg3ToMCValue(0));

    Socket target(api, SocketFamily::InternetV4, SocketType::Datagram);
    std::chrono::milliseconds timeout(0);
    EXPECT_EQ(timeout, target.GetReceiveTimeout());
}

TEST_F(SocketTest, SetReceiveTimeout)
{
    testing::SocketAPIMock api;

    EXPECT_CALL(api, Open(SocketFamily::InternetV4, SocketType::Datagram, _)).Times(1);
    EXPECT_CALL(api, Close(_)).Times(1);
    EXPECT_CALL(api, GetSocketOption(_, SocketOptionLevel::Socket, SocketOption::ReceiveTimeout, _, _)).WillOnce(SetArg3ToMCValue(0)).WillOnce(SetArg3ToMCValue(1)).WillOnce(SetArg3ToMCValue(0));
    EXPECT_CALL(api, SetSocketOption(_, SocketOptionLevel::Socket, SocketOption::ReceiveTimeout, _, _)).Times(2);

    Socket target(api, SocketFamily::InternetV4, SocketType::Datagram);
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
    testing::SocketAPIMock api;

    EXPECT_CALL(api, Open(SocketFamily::InternetV4, SocketType::Datagram, _)).Times(1);
    EXPECT_CALL(api, Close(_)).Times(1);
    EXPECT_CALL(api, GetSocketOption(_, SocketOptionLevel::Socket, SocketOption::SendTimeout, _, _)).WillOnce(SetArg3ToMCValue(0));

    Socket target(api, SocketFamily::InternetV4, SocketType::Datagram);
    std::chrono::milliseconds timeout(0);
    EXPECT_EQ(timeout, target.GetSendTimeout());
}

TEST_F(SocketTest, SetSendTimeout)
{
    testing::SocketAPIMock api;

    EXPECT_CALL(api, Open(SocketFamily::InternetV4, SocketType::Datagram, _)).Times(1);
    EXPECT_CALL(api, Close(_)).Times(1);
    EXPECT_CALL(api, GetSocketOption(_, SocketOptionLevel::Socket, SocketOption::SendTimeout, _, _)).WillOnce(SetArg3ToMCValue(0)).WillOnce(SetArg3ToMCValue(1)).WillOnce(SetArg3ToMCValue(0));
    EXPECT_CALL(api, SetSocketOption(_, SocketOptionLevel::Socket, SocketOption::SendTimeout, _, _)).Times(2);

    Socket target(api, SocketFamily::InternetV4, SocketType::Datagram);
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
    SocketAPI api;
    Socket acceptorSocket(api, SocketFamily::InternetV4, SocketType::Stream);
    sockaddr_in serverAddress {};
    FillAddress(serverAddress, TestPort, IPV4Address::LocalHost.GetUInt32());

    acceptorSocket.Bind(reinterpret_cast<sockaddr *>(&serverAddress), sizeof(serverAddress));
    acceptorSocket.Listen(1);
    Socket newSocket(api);
    sockaddr_in clientAddress {};
    socklen_t clientAddressSize = sizeof(clientAddress);
    accepted = acceptorSocket.Accept(newSocket, reinterpret_cast<sockaddr *>(&clientAddress), &clientAddressSize, 5000);
    const std::size_t BufferSize = 10;
    std::uint8_t buffer[BufferSize];
    std::size_t bytesReceived = newSocket.Receive(buffer, BufferSize, 0);
    newSocket.Send(buffer, bytesReceived, 0);
    // Wait for client to close connection, otherwise we'll end up in TIME_WAIT status
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    return accepted;
}

TEST_F(SocketTest, ConnectAcceptSendReceiveTCP)
{
    SocketAPI api;
    Socket clientSocket(api, SocketFamily::InternetV4, SocketType::Stream);
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
        // Make sure to close client before server ends, otherwise we'll end up in TIME_WAIT status
        clientSocket.Close();
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
    SocketAPI api;
    Socket serverSocket(api, SocketFamily::InternetV4, SocketType::Datagram);
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
    SocketAPI api;
    Socket clientSocket(api, SocketFamily::InternetV4, SocketType::Datagram);
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
    SocketAPI api;
    Socket clientSocket(api, SocketFamily::InternetV4, SocketType::Datagram);
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
    testing::SocketAPIMock api;
    Socket target(api);

    EXPECT_EQ("-1", serialization::Serialize(target, socklen_t {0}));

    SocketHandle handle = 1234;
    target.SetHandle(handle);
    EXPECT_EQ("1234", serialization::Serialize(target, 0));

    target.SetHandle(InvalidHandleValue);
}

TEST_F(SocketTest, SerializeSockAddrSize)
{
    sockaddr_in address {};
    address.sin_family = AF_INET;

    EXPECT_EQ("addressFamily=2, size=16", serialization::Serialize(reinterpret_cast<sockaddr *>(&address), static_cast<socklen_t>(sizeof(address))));
}

TEST_F(SocketTest, SerializeSockAddrSizePtr)
{
    sockaddr_in address {};
    address.sin_family = AF_INET;
    socklen_t addressSize = sizeof(address);

    EXPECT_EQ("addressFamily=2, size=16", serialization::Serialize(reinterpret_cast<sockaddr *>(&address), &addressSize));
}

} // namespace testing
} // namespace network
