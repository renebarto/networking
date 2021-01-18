#include "GoogleTest.h"

#include "network/IPV6TCPSocket.h"

#include "network-osal/SocketAPI.h"
#include "tracing/ScopedTracing.h"
#include "tracing/Tracing.h"
#include "Utility.h"

namespace network {

class IPV6TCPSocketTest : public ::testing::Test
{
public:
    tracing::CategorySet<tracing::TraceCategory> m_savedTraceFilter;

    IPV6TCPSocketTest()
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

TEST_F(IPV6TCPSocketTest, ConstructDefault)
{
    SocketAPI api;
    IPV6TCPSocket target(api);
    EXPECT_NE(InvalidHandleValue, target.GetHandle());
    EXPECT_TRUE(target.IsOpen());
    EXPECT_EQ(SocketFamily::InternetV6, target.Family());
    EXPECT_EQ(SocketType::Stream, target.Type());
}

TEST_F(IPV6TCPSocketTest, ConstructCopy)
{
    SocketAPI api;
    IPV6TCPSocket target(api);
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
    SocketAPI api;
    IPV6TCPSocket target(api);
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
    SocketAPI api;
    IPV6TCPSocket target(api);
    auto handle = target.GetHandle();

    IPV6TCPSocket newSocket(api);
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
    SocketAPI api;
    IPV6TCPSocket target(api);
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
    SocketAPI api;
    IPV6TCPSocket target(api);
    target.Close();

    target.Open();
    EXPECT_TRUE(target.IsOpen());
    EXPECT_EQ(SocketFamily::InternetV6, target.Family());
    EXPECT_EQ(SocketType::Stream, target.Type());
}

TEST_F(IPV6TCPSocketTest, Close)
{
    SocketAPI api;
    IPV6TCPSocket target(api);
    EXPECT_TRUE(target.IsOpen());
    target.Close();
    EXPECT_FALSE(target.IsOpen());
    EXPECT_EQ(SocketFamily::Any, target.Family());
    EXPECT_EQ(SocketType::None, target.Type());
}

TEST_F(IPV6TCPSocketTest, GetOptionWithLevel)
{
    SocketAPI api;
    IPV6TCPSocket target(api);
    linger value { };
    socklen_t size = static_cast<socklen_t>(sizeof(value));
    target.GetSocketOptionWithLevel(SocketOptionLevel::Socket, SocketOption::Linger, &value, &size);
    EXPECT_EQ(0, value.l_onoff);
    EXPECT_EQ(0, value.l_linger);
}

TEST_F(IPV6TCPSocketTest, SetOptionWithLevel)
{
    SocketAPI api;
    IPV6TCPSocket target(api);
    linger value { 1, 0 };
    socklen_t size = static_cast<socklen_t>(sizeof(value));
    linger actual;
    target.SetSocketOptionWithLevel(SocketOptionLevel::Socket, SocketOption::Linger, &value, size);
    target.GetSocketOptionWithLevel(SocketOptionLevel::Socket, SocketOption::Linger, &actual, &size);
    EXPECT_EQ(value.l_onoff, actual.l_onoff);
    EXPECT_EQ(value.l_linger, actual.l_linger);
}

TEST_F(IPV6TCPSocketTest, GetOption)
{
    SocketAPI api;
    IPV6TCPSocket target(api);
    linger value { };
    socklen_t size = static_cast<socklen_t>(sizeof(value));
    target.GetSocketOption(SocketOption::Linger, &value, &size);
    EXPECT_EQ(0, value.l_onoff);
    EXPECT_EQ(0, value.l_linger);
}

TEST_F(IPV6TCPSocketTest, SetOption)
{
    SocketAPI api;
    IPV6TCPSocket target(api);
    linger value { 1, 0 };
    socklen_t size = static_cast<socklen_t>(sizeof(value));
    linger actual;
    target.SetSocketOption(SocketOption::Linger, &value, size);
    target.GetSocketOption(SocketOption::Linger, &actual, &size);
    EXPECT_EQ(value.l_onoff, actual.l_onoff);
    EXPECT_EQ(value.l_linger, actual.l_linger);
}

TEST_F(IPV6TCPSocketTest, GetSetSocketOptionBool)
{
    SocketAPI api;
    IPV6TCPSocket target(api);
    target.SetSocketOptionBool(SocketOption::KeepAlive, true);
    EXPECT_TRUE(target.GetSocketOptionBool(SocketOption::KeepAlive));
}

TEST_F(IPV6TCPSocketTest, GetSocketOptionInt)
{
    SocketAPI api;
    IPV6TCPSocket target(api);
    EXPECT_NE(0, target.GetSocketOptionInt(SocketOption::ReceiveBuffer));
}

TEST_F(IPV6TCPSocketTest, SetSocketOptionInt)
{
    SocketAPI api;
    IPV6TCPSocket target(api);
    // Must be larger than 2304
    int receiveBufferSize = 4096;
    EXPECT_NE(0, target.GetSocketOptionInt(SocketOption::ReceiveBuffer));
    target.SetSocketOptionInt(SocketOption::ReceiveBuffer, receiveBufferSize);
    EXPECT_LE(receiveBufferSize, target.GetSocketOptionInt(SocketOption::ReceiveBuffer));
}

TEST_F(IPV6TCPSocketTest, GetBlockingMode)
{
    SocketAPI api;
    IPV6TCPSocket target(api);
    EXPECT_TRUE(target.GetBlockingMode());
}

TEST_F(IPV6TCPSocketTest, SetBlockingMode)
{
    SocketAPI api;
    IPV6TCPSocket target(api);
    EXPECT_TRUE(target.GetBlockingMode());
    target.SetBlockingMode(false);
    EXPECT_FALSE(target.GetBlockingMode());
    target.SetBlockingMode(true);
    EXPECT_TRUE(target.GetBlockingMode());
}

TEST_F(IPV6TCPSocketTest, GetReuseAddress)
{
    SocketAPI api;
    IPV6TCPSocket target(api);
    EXPECT_FALSE(target.GetReuseAddress());
}

TEST_F(IPV6TCPSocketTest, SetReuseAddress)
{
    SocketAPI api;
    IPV6TCPSocket target(api);
    EXPECT_FALSE(target.GetReuseAddress());
    target.SetReuseAddress(true);
    EXPECT_TRUE(target.GetReuseAddress());
    target.SetReuseAddress(false);
    EXPECT_FALSE(target.GetReuseAddress());
}

TEST_F(IPV6TCPSocketTest, GetReceiveTimeout)
{
    SocketAPI api;
    IPV6TCPSocket target(api);
    std::chrono::milliseconds timeout(0);
    EXPECT_EQ(timeout, target.GetReceiveTimeout());
}

TEST_F(IPV6TCPSocketTest, SetReceiveTimeout)
{
    SocketAPI api;
    IPV6TCPSocket target(api);
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
    SocketAPI api;
    IPV6TCPSocket target(api);
    std::chrono::milliseconds timeout(0);
    EXPECT_EQ(timeout, target.GetSendTimeout());
}

TEST_F(IPV6TCPSocketTest, SetSendTimeout)
{
    SocketAPI api;
    IPV6TCPSocket target(api);
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
    SocketAPI api;
    bool accepted {};
    SCOPEDTRACE([] () { return "TCP Accept Send Recv thread"; }, [&]{
        return serialization::Serialize(accepted);
    });
    IPV6TCPSocket acceptorSocket(api);

    acceptorSocket.Bind(IPV6EndPoint(TestPort));
    acceptorSocket.Listen(1);
    IPV6TCPSocket newSocket(api);
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

TEST_F(IPV6TCPSocketTest, ConnectAcceptSendReceiveTCP)
{
    SocketAPI api;
    IPV6TCPSocket clientSocket(api);
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
        // Make sure to close client before server ends, otherwise we'll end up in TIME_WAIT status
        clientSocket.Close();
    }
    acceptorThread.WaitForDeath();
    bool accepted = acceptorThread.GetResult();
    EXPECT_TRUE(accepted);
}

} // namespace network