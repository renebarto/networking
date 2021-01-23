#include "GoogleTest.h"

#include "network/IPV6UDPSocket.h"

#include "network-osal/SocketAPI.h"
#include "tracing/ScopedTracing.h"
#include "tracing/Logging.h"
#include "utility/GenericError.h"
#include "Utility.h"

namespace network {

class IPV6UDPSocketTest : public ::testing::Test
{
public:
    tracing::CategorySet<tracing::TraceCategory> m_savedTraceFilter;

    IPV6UDPSocketTest()
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

TEST_F(IPV6UDPSocketTest, ConstructDefault)
{
    SocketAPI api;
    IPV6UDPSocket target(api);
    target.Open();
    EXPECT_NE(InvalidHandleValue, target.GetHandle());
    EXPECT_TRUE(target.IsOpen());
    EXPECT_EQ(SocketFamily::InternetV6, target.Family());
    EXPECT_EQ(SocketType::Datagram, target.Type());
    EXPECT_EQ(SocketProtocol::IP, target.Protocol());
}

TEST_F(IPV6UDPSocketTest, ConstructMove)
{
    SocketAPI api;
    IPV6UDPSocket target(api);
    target.Open();
    auto handle = target.GetHandle();

    IPV6UDPSocket newSocket(std::move(target));
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

TEST_F(IPV6UDPSocketTest, AssignMove)
{
    SocketAPI api;
    IPV6UDPSocket target(api);
    target.Open();
    auto handle = target.GetHandle();

    IPV6UDPSocket newSocket(api);
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

TEST_F(IPV6UDPSocketTest, GetSetHandle)
{
    SocketAPI api;
    IPV6UDPSocket target(api);
    target.Open();
    target.Close();

    SocketHandle handle = 1234;
    target.SetHandle(handle);
    EXPECT_EQ(handle, target.GetHandle());
    EXPECT_TRUE(target.IsOpen());

    target.SetHandle(InvalidHandleValue);
    EXPECT_EQ(InvalidHandleValue, target.GetHandle());
    EXPECT_FALSE(target.IsOpen());
}

TEST_F(IPV6UDPSocketTest, Open)
{
    SocketAPI api;
    IPV6UDPSocket target(api);
    target.Open();
    EXPECT_TRUE(target.IsOpen());
    EXPECT_EQ(SocketFamily::InternetV6, target.Family());
    EXPECT_EQ(SocketType::Datagram, target.Type());
    EXPECT_EQ(SocketProtocol::IP, target.Protocol());
}

TEST_F(IPV6UDPSocketTest, Close)
{
    SocketAPI api;
    IPV6UDPSocket target(api);
    target.Open();
    EXPECT_TRUE(target.IsOpen());
    target.Close();
    EXPECT_FALSE(target.IsOpen());
    EXPECT_EQ(SocketFamily::InternetV6, target.Family());
    EXPECT_EQ(SocketType::Datagram, target.Type());
    EXPECT_EQ(SocketProtocol::IP, target.Protocol());
}

TEST_F(IPV6UDPSocketTest, GetOptionWithLevel)
{
    SocketAPI api;
    IPV6UDPSocket target(api);
    target.Open();
    socklen_t size = static_cast<socklen_t>(sizeof(int));
    int value;
    target.GetSocketOptionWithLevel(SocketOptionLevel::Socket, SocketOption::Broadcast, &value, &size);
    EXPECT_FALSE(value != 0);
}

TEST_F(IPV6UDPSocketTest, SetOptionWithLevel)
{
    SocketAPI api;
    IPV6UDPSocket target(api);
    target.Open();
    EXPECT_FALSE(target.GetSocketOptionBool(SocketOption::Broadcast));
    socklen_t size = static_cast<socklen_t>(sizeof(int));
    int value = 1;
    int actual;
    target.SetSocketOptionWithLevel(SocketOptionLevel::Socket, SocketOption::Broadcast, &value, size);
    target.GetSocketOptionWithLevel(SocketOptionLevel::Socket, SocketOption::Broadcast, &actual, &size);
    EXPECT_EQ(value, actual);
}

TEST_F(IPV6UDPSocketTest, GetOption)
{
    SocketAPI api;
    IPV6UDPSocket target(api);
    target.Open();
    socklen_t size = static_cast<socklen_t>(sizeof(int));
    int value;
    target.GetSocketOption(SocketOption::Broadcast, &value, &size);
    EXPECT_FALSE(value != 0);
}

TEST_F(IPV6UDPSocketTest, SetOption)
{
    SocketAPI api;
    IPV6UDPSocket target(api);
    target.Open();
    EXPECT_FALSE(target.GetSocketOptionBool(SocketOption::Broadcast));
    socklen_t size = static_cast<socklen_t>(sizeof(int));
    int value = 1;
    int actual;
    target.SetSocketOption(SocketOption::Broadcast, &value, size);
    target.GetSocketOption(SocketOption::Broadcast, &actual, &size);
    EXPECT_EQ(value, actual);
}

TEST_F(IPV6UDPSocketTest, GetSocketOptionBool)
{
    SocketAPI api;
    IPV6UDPSocket target(api);
    target.Open();
    EXPECT_FALSE(target.GetSocketOptionBool(SocketOption::Broadcast));
}

TEST_F(IPV6UDPSocketTest, SetSocketOptionBool)
{
    SocketAPI api;
    IPV6UDPSocket target(api);
    target.Open();
    EXPECT_FALSE(target.GetSocketOptionBool(SocketOption::Broadcast));
    target.SetSocketOptionBool(SocketOption::Broadcast, true);
    EXPECT_TRUE(target.GetSocketOptionBool(SocketOption::Broadcast));
}

TEST_F(IPV6UDPSocketTest, GetSocketOptionInt)
{
    SocketAPI api;
    IPV6UDPSocket target(api);
    target.Open();
    EXPECT_EQ(0, target.GetSocketOptionInt(SocketOption::Broadcast));
}

TEST_F(IPV6UDPSocketTest, SetSocketOptionInt)
{
    int enableBroadcast = 1;
    SocketAPI api;
    IPV6UDPSocket target(api);
    target.Open();
    EXPECT_FALSE(target.GetSocketOptionInt(SocketOption::Broadcast));
    target.SetSocketOptionInt(SocketOption::Broadcast, enableBroadcast);
    EXPECT_EQ(enableBroadcast, target.GetSocketOptionInt(SocketOption::Broadcast));
}

TEST_F(IPV6UDPSocketTest, GetBroadcastOption)
{
    SocketAPI api;
    IPV6UDPSocket target(api);
    target.Open();
    EXPECT_FALSE(target.GetBroadcastOption());
}

TEST_F(IPV6UDPSocketTest, SetBroadcastOption)
{
    SocketAPI api;
    IPV6UDPSocket target(api);
    target.Open();
    EXPECT_FALSE(target.GetBroadcastOption());
    target.SetBroadcastOption(true);
    EXPECT_TRUE(target.GetBroadcastOption());
}

TEST_F(IPV6UDPSocketTest, GetBlockingMode)
{
    SocketAPI api;
    IPV6UDPSocket target(api);
    target.Open();
    EXPECT_TRUE(target.GetBlockingMode());
}

TEST_F(IPV6UDPSocketTest, SetBlockingMode)
{
    SocketAPI api;
    IPV6UDPSocket target(api);
    target.Open();
    EXPECT_TRUE(target.GetBlockingMode());
    target.SetBlockingMode(false);
    EXPECT_FALSE(target.GetBlockingMode());
    target.SetBlockingMode(true);
    EXPECT_TRUE(target.GetBlockingMode());
}

TEST_F(IPV6UDPSocketTest, GetReuseAddress)
{
    SocketAPI api;
    IPV6UDPSocket target(api);
    target.Open();
    EXPECT_FALSE(target.GetReuseAddress());
}

TEST_F(IPV6UDPSocketTest, SetReuseAddress)
{
    SocketAPI api;
    IPV6UDPSocket target(api);
    target.Open();
    EXPECT_FALSE(target.GetReuseAddress());
    target.SetReuseAddress(true);
    EXPECT_TRUE(target.GetReuseAddress());
    target.SetReuseAddress(false);
    EXPECT_FALSE(target.GetReuseAddress());
}

TEST_F(IPV6UDPSocketTest, GetReceiveTimeout)
{
    SocketAPI api;
    IPV6UDPSocket target(api);
    target.Open();
    std::chrono::milliseconds timeout(0);
    EXPECT_EQ(timeout, target.GetReceiveTimeout());
}

TEST_F(IPV6UDPSocketTest, SetReceiveTimeout)
{
    SocketAPI api;
    IPV6UDPSocket target(api);
    target.Open();
    std::chrono::milliseconds timeout(0);
    std::chrono::milliseconds timeoutNew(1000);
    EXPECT_EQ(timeout, target.GetReceiveTimeout());
    target.SetReceiveTimeout(timeoutNew);
    EXPECT_EQ(timeoutNew, target.GetReceiveTimeout());
    target.SetReceiveTimeout(timeout);
    EXPECT_EQ(timeout, target.GetReceiveTimeout());
}

TEST_F(IPV6UDPSocketTest, GetSendTimeout)
{
    SocketAPI api;
    IPV6UDPSocket target(api);
    target.Open();
    std::chrono::milliseconds timeout(0);
    EXPECT_EQ(timeout, target.GetSendTimeout());
}

TEST_F(IPV6UDPSocketTest, SetSendTimeout)
{
    SocketAPI api;
    IPV6UDPSocket target(api);
    target.Open();
    std::chrono::milliseconds timeout(0);
    std::chrono::milliseconds timeoutNew(1000);
    EXPECT_EQ(timeout, target.GetSendTimeout());
    target.SetSendTimeout(timeoutNew);
    EXPECT_EQ(timeoutNew, target.GetSendTimeout());
    target.SetSendTimeout(timeout);
    EXPECT_EQ(timeout, target.GetSendTimeout());
}

bool IPV6UDPSocketUDPServerThread()
{
    SocketAPI api;
    bool ok {};
    try
    {
        SCOPEDTRACE([] () { return "UDP Send Recv thread"; }, [&]{
            return serialization::Serialize(ok);
        });
        IPV6UDPSocket serverSocket(api);
        IPV6EndPoint serverAddress(TestPort);
        serverSocket.Open();
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

TEST_F(IPV6UDPSocketTest, SendReceiveUDPConnected)
{
    SocketAPI api;
    IPV6UDPSocket clientSocket(api);
    clientSocket.Open();
    IPV6EndPoint serverAddress(IPV6Address::LocalHost, TestPort);

    core::TypedReturnThread<bool> serverThread(IPV6UDPSocketUDPServerThread);
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

TEST_F(IPV6UDPSocketTest, SendReceiveUDPConnectionless)
{
    SocketAPI api;
    IPV6UDPSocket clientSocket(api);
    clientSocket.Open();
    IPV6EndPoint serverAddress(IPV6Address::LocalHost, TestPort);

    core::TypedReturnThread<bool> serverThread(IPV6UDPSocketUDPServerThread);
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