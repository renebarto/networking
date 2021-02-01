#include "GoogleTest.h"

#include "network/IPV4TCPSocket.h"

#include "network-osal/SocketAPI.h"
#include "tracing/ScopedTracing.h"
#include "tracing/Tracing.h"
#include "Utility.h"

namespace network {

class IPV4TCPSocketTest : public ::testing::Test
{
public:
    tracing::CategorySet<tracing::TraceCategory> m_savedTraceFilter;

    IPV4TCPSocketTest()
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

TEST_F(IPV4TCPSocketTest, ConstructDefault)
{
    SocketAPI api;
    IPV4TCPSocket target(api);
    EXPECT_EQ(InvalidHandleValue, target.GetHandle());
    EXPECT_FALSE(target.IsOpen());
    EXPECT_EQ(SocketFamily::InternetV4, target.Family());
    EXPECT_EQ(SocketType::Stream, target.Type());
    EXPECT_EQ(SocketProtocol::IP, target.Protocol());
}

TEST_F(IPV4TCPSocketTest, ConstructMove)
{
    SocketAPI api;
    IPV4TCPSocket target(api);
    target.Open();
    auto handle = target.GetHandle();

    IPV4TCPSocket newSocket(std::move(target));
    EXPECT_EQ(InvalidHandleValue, target.GetHandle());
    EXPECT_FALSE(target.IsOpen());
    EXPECT_EQ(handle, newSocket.GetHandle());
    EXPECT_TRUE(newSocket.IsOpen());
    EXPECT_EQ(SocketFamily::InternetV4, target.Family());
    EXPECT_EQ(SocketType::Stream, target.Type());
    EXPECT_EQ(SocketProtocol::IP, target.Protocol());
    EXPECT_EQ(SocketFamily::InternetV4, newSocket.Family());
    EXPECT_EQ(SocketType::Stream, newSocket.Type());
    EXPECT_EQ(SocketProtocol::IP, newSocket.Protocol());
}

TEST_F(IPV4TCPSocketTest, AssignMove)
{
    SocketAPI api;
    IPV4TCPSocket target(api);
    target.Open();
    auto handle = target.GetHandle();

    IPV4TCPSocket newSocket(api);
    newSocket = std::move(target);
    EXPECT_EQ(InvalidHandleValue, target.GetHandle());
    EXPECT_FALSE(target.IsOpen());
    EXPECT_EQ(handle, newSocket.GetHandle());
    EXPECT_TRUE(newSocket.IsOpen());
    EXPECT_EQ(SocketFamily::InternetV4, target.Family());
    EXPECT_EQ(SocketType::Stream, target.Type());
    EXPECT_EQ(SocketProtocol::IP, target.Protocol());
    EXPECT_EQ(SocketFamily::InternetV4, newSocket.Family());
    EXPECT_EQ(SocketType::Stream, newSocket.Type());
    EXPECT_EQ(SocketProtocol::IP, newSocket.Protocol());
}

TEST_F(IPV4TCPSocketTest, GetSetHandle)
{
    SocketAPI api;
    IPV4TCPSocket target(api);
    target.Close();

    SocketHandle handle = 1234;
    target.SetHandle(handle);
    EXPECT_EQ(handle, target.GetHandle());
    EXPECT_TRUE(target.IsOpen());

    target.SetHandle(InvalidHandleValue);
    EXPECT_EQ(InvalidHandleValue, target.GetHandle());
    EXPECT_FALSE(target.IsOpen());
}

TEST_F(IPV4TCPSocketTest, Open)
{
    SocketAPI api;
    IPV4TCPSocket target(api);
    target.Close();
    target.Open();
    EXPECT_TRUE(target.IsOpen());
    EXPECT_EQ(SocketFamily::InternetV4, target.Family());
    EXPECT_EQ(SocketType::Stream, target.Type());
    EXPECT_EQ(SocketProtocol::IP, target.Protocol());
}

TEST_F(IPV4TCPSocketTest, Close)
{
    SocketAPI api;
    IPV4TCPSocket target(api);
    target.Open();
    EXPECT_TRUE(target.IsOpen());
    target.Close();
    EXPECT_FALSE(target.IsOpen());
    EXPECT_EQ(SocketFamily::InternetV4, target.Family());
    EXPECT_EQ(SocketType::Stream, target.Type());
    EXPECT_EQ(SocketProtocol::IP, target.Protocol());
}

TEST_F(IPV4TCPSocketTest, GetOptionWithLevel)
{
    SocketAPI api;
    IPV4TCPSocket target(api);
    target.Open();
    linger value { };
    socklen_t size = static_cast<socklen_t>(sizeof(value));
    target.GetSocketOptionWithLevel(SocketOptionLevel::Socket, SocketOption::Linger, &value, &size);
    EXPECT_EQ(0, value.l_onoff);
    EXPECT_EQ(0, value.l_linger);
}

TEST_F(IPV4TCPSocketTest, SetOptionWithLevel)
{
    SocketAPI api;
    IPV4TCPSocket target(api);
    target.Open();
    linger value { 1, 0 };
    socklen_t size = static_cast<socklen_t>(sizeof(value));
    linger actual;
    target.SetSocketOptionWithLevel(SocketOptionLevel::Socket, SocketOption::Linger, &value, size);
    target.GetSocketOptionWithLevel(SocketOptionLevel::Socket, SocketOption::Linger, &actual, &size);
    EXPECT_EQ(value.l_onoff, actual.l_onoff);
    EXPECT_EQ(value.l_linger, actual.l_linger);
}

TEST_F(IPV4TCPSocketTest, GetOption)
{
    SocketAPI api;
    IPV4TCPSocket target(api);
    target.Open();
    linger value { };
    socklen_t size = static_cast<socklen_t>(sizeof(value));
    target.GetSocketOption(SocketOption::Linger, &value, &size);
    EXPECT_EQ(0, value.l_onoff);
    EXPECT_EQ(0, value.l_linger);
}

TEST_F(IPV4TCPSocketTest, SetOption)
{
    SocketAPI api;
    IPV4TCPSocket target(api);
    target.Open();
    linger value { 1, 0 };
    socklen_t size = static_cast<socklen_t>(sizeof(value));
    linger actual;
    target.SetSocketOption(SocketOption::Linger, &value, size);
    target.GetSocketOption(SocketOption::Linger, &actual, &size);
    EXPECT_EQ(value.l_onoff, actual.l_onoff);
    EXPECT_EQ(value.l_linger, actual.l_linger);
}

TEST_F(IPV4TCPSocketTest, GetSetSocketOptionBool)
{
    SocketAPI api;
    IPV4TCPSocket target(api);
    target.Open();
    target.SetSocketOptionBool(SocketOption::KeepAlive, true);
    EXPECT_TRUE(target.GetSocketOptionBool(SocketOption::KeepAlive));
}

TEST_F(IPV4TCPSocketTest, GetSocketOptionInt)
{
    SocketAPI api;
    IPV4TCPSocket target(api);
    target.Open();
    EXPECT_NE(0, target.GetSocketOptionInt(SocketOption::ReceiveBuffer));
}

TEST_F(IPV4TCPSocketTest, SetSocketOptionInt)
{
    SocketAPI api;
    IPV4TCPSocket target(api);
    target.Open();
    // Must be larger than 2304
    int receiveBufferSize = 4096;
    EXPECT_NE(0, target.GetSocketOptionInt(SocketOption::ReceiveBuffer));
    target.SetSocketOptionInt(SocketOption::ReceiveBuffer, receiveBufferSize);
    EXPECT_LE(receiveBufferSize, target.GetSocketOptionInt(SocketOption::ReceiveBuffer));
}

TEST_F(IPV4TCPSocketTest, GetBlockingMode)
{
    SocketAPI api;
    IPV4TCPSocket target(api);
    target.Open();
    EXPECT_TRUE(target.GetBlockingMode());
}

TEST_F(IPV4TCPSocketTest, SetBlockingMode)
{
    SocketAPI api;
    IPV4TCPSocket target(api);
    target.Open();
    EXPECT_TRUE(target.GetBlockingMode());
    target.SetBlockingMode(false);
    EXPECT_FALSE(target.GetBlockingMode());
    target.SetBlockingMode(true);
    EXPECT_TRUE(target.GetBlockingMode());
}

TEST_F(IPV4TCPSocketTest, GetReuseAddress)
{
    SocketAPI api;
    IPV4TCPSocket target(api);
    target.Open();
    EXPECT_FALSE(target.GetReuseAddress());
}

TEST_F(IPV4TCPSocketTest, SetReuseAddress)
{
    SocketAPI api;
    IPV4TCPSocket target(api);
    target.Open();
    EXPECT_FALSE(target.GetReuseAddress());
    target.SetReuseAddress(true);
    EXPECT_TRUE(target.GetReuseAddress());
    target.SetReuseAddress(false);
    EXPECT_FALSE(target.GetReuseAddress());
}

TEST_F(IPV4TCPSocketTest, GetReceiveTimeout)
{
    SocketAPI api;
    IPV4TCPSocket target(api);
    target.Open();
    std::chrono::milliseconds timeout(0);
    EXPECT_EQ(timeout, target.GetReceiveTimeout());
}

TEST_F(IPV4TCPSocketTest, SetReceiveTimeout)
{
    SocketAPI api;
    IPV4TCPSocket target(api);
    target.Open();
    std::chrono::milliseconds timeout(0);
    std::chrono::milliseconds timeoutNew(1000);
    EXPECT_EQ(timeout, target.GetReceiveTimeout());
    target.SetReceiveTimeout(timeoutNew);
    EXPECT_EQ(timeoutNew, target.GetReceiveTimeout());
    target.SetReceiveTimeout(timeout);
    EXPECT_EQ(timeout, target.GetReceiveTimeout());
}

TEST_F(IPV4TCPSocketTest, GetSendTimeout)
{
    SocketAPI api;
    IPV4TCPSocket target(api);
    target.Open();
    std::chrono::milliseconds timeout(0);
    EXPECT_EQ(timeout, target.GetSendTimeout());
}

TEST_F(IPV4TCPSocketTest, SetSendTimeout)
{
    SocketAPI api;
    IPV4TCPSocket target(api);
    target.Open();
    std::chrono::milliseconds timeout(0);
    std::chrono::milliseconds timeoutNew(1000);
    EXPECT_EQ(timeout, target.GetSendTimeout());
    target.SetSendTimeout(timeoutNew);
    EXPECT_EQ(timeoutNew, target.GetSendTimeout());
    target.SetSendTimeout(timeout);
    EXPECT_EQ(timeout, target.GetSendTimeout());
}

bool IPV4TCPSocketTCPAcceptThread()
{
    SocketAPI api;
    bool accepted {};
    SCOPEDTRACE([] () { return "TCP Accept Send Recv thread"; }, [&]{
        return serialization::Serialize(accepted);
    });
    IPV4TCPSocket acceptorSocket(api);
    acceptorSocket.Open();
    acceptorSocket.Bind(IPV4EndPoint(TestPort));
    acceptorSocket.Listen(1);
    IPV4TCPSocket newSocket(api);
    IPV4EndPoint clientAddress;
    accepted = acceptorSocket.Accept(newSocket, clientAddress, 5000);
    const std::size_t BufferSize = 10;
    std::uint8_t buffer[BufferSize];
    std::size_t bytesReceived = newSocket.Receive(buffer, BufferSize, 0);
    newSocket.Send(buffer, bytesReceived, 0);
    // Wait for client to close connection, otherwise we'll end up in TIME_WAIT status
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    return accepted;
}

TEST_F(IPV4TCPSocketTest, ConnectAcceptSendReceiveTCP)
{
    SocketAPI api;
    IPV4TCPSocket clientSocket(api);
    clientSocket.Open();
    IPV4EndPoint serverAddress(IPV4Address::LocalHost, TestPort);

    core::TypedReturnThread<bool> acceptorThread(IPV4TCPSocketTCPAcceptThread);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    bool connected = clientSocket.Connect(serverAddress, 5000);
    EXPECT_TRUE(connected);
    if (connected)
    {
        const std::size_t BufferSize = 10;
        std::uint8_t bufferOut[BufferSize] = { 'H', 'e', 'l', 'l', 'o', 'W', 'o', 'r', 'l', 'd'};
        std::uint8_t bufferIn[BufferSize];
        std::size_t bytesSent = clientSocket.Send(bufferOut, BufferSize, 0);
        EXPECT_EQ(BufferSize, bytesSent);
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