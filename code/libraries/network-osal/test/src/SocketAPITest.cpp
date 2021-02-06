#include "GoogleTest.h"

#include "network-osal/SocketAPI.h"

#include <algorithm>
#include "osal/Utilities.h"
#include "core/Thread.h"
#include "network-osal/IPV4Address.h"
#include "network-osal/Network.h"
#include "network-osal/Socket.h"
#include "tracing/Logging.h"
#include "tracing/Tracing.h"
#include "utility/Error.h"
#include "Utility.h"

#if defined(PLATFORM_LINUX)

#include <fcntl.h>
#include <poll.h>

#elif defined(PLATFORM_WINDOWS)

#ifdef min
#undef min
#endif

#else

#error Unsupported platform

#endif

namespace network {
namespace testing {

static const SocketTimeout TimeWait = 1000;

static void FillAddress(sockaddr_in & address, std::uint16_t port, std::uint32_t ipAddress)
{
    address.sin_family = AF_INET;
    address.sin_port = osal::SwapBytes(port);
    address.sin_addr.s_addr = ipAddress;
}

class SocketAPITest : public ::testing::Test
{
public:
    tracing::CategorySet<tracing::TraceCategory> savedTraceFilter;
    SocketHandle handle;
    SocketAPI api;

    SocketAPITest()
        : savedTraceFilter()
        , handle(InvalidHandleValue)
        , api()
    {}

    void SetUp() override
    {
        savedTraceFilter = tracing::GetDefaultTraceFilter();
        tracing::SetDefaultTraceFilter(tracing::TraceCategory::Message);
    }
    void TearDown() override
    {
        if (handle != InvalidHandleValue)
            api.Close(handle);
        handle = InvalidHandleValue;
        tracing::SetDefaultTraceFilter(savedTraceFilter);
    }
};

TEST_F(SocketAPITest, OpenUDP)
{
    handle = api.Open(SocketFamily::InternetV4, SocketType::Datagram, SocketProtocol::IP);

    EXPECT_NE(InvalidHandleValue, handle);
}

TEST_F(SocketAPITest, OpenTCP)
{
    handle = api.Open(SocketFamily::InternetV4, SocketType::Stream, SocketProtocol::IP);

    EXPECT_NE(InvalidHandleValue, handle);
}

TEST_F(SocketAPITest, OpenCloseUDP)
{
    handle = api.Open(SocketFamily::InternetV4, SocketType::Datagram, SocketProtocol::IP);

    EXPECT_NE(InvalidHandleValue, handle);
    api.Close(handle);
}

TEST_F(SocketAPITest, OpenCloseTCP)
{
    handle = api.Open(SocketFamily::InternetV4, SocketType::Stream, SocketProtocol::IP);

    EXPECT_NE(InvalidHandleValue, handle);
    api.Close(handle);
}

bool SetBlockingMode(SocketAPI & api, SocketHandle handle, bool blocking)
{
    TraceMessage(__FILE__, __LINE__, __func__, (blocking ? "Set blocking mode" : "Set non blocking mode"));
    if (-1 == api.SetBlockingMode(handle, blocking))
    {
        int errorCode = GetError();
        tracing::Logging::Error(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "SetBlockingMode() failed"));
        return false;
    }
    return true;
}

bool ExchangeDataNonConnected(SocketAPI & api, SocketHandle handle)
{
    sockaddr_in serverAddress {};
    FillAddress(serverAddress, TestPort, IPV4Address::LocalHost.GetUInt32());

    const int Size = 10;
    std::uint8_t bufferOut[Size] = { 'H', 'e', 'l', 'l', 'o', 'W', 'o', 'r', 'l', 'd'};
    std::uint8_t bufferIn[Size];
    int numBytesSent = api.SendTo(handle, bufferOut, Size, 0, reinterpret_cast<const sockaddr *>(&serverAddress), sizeof(serverAddress));
    if (numBytesSent == -1)
    {
        int errorCode = GetError();
        tracing::Logging::Error(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "Client SendTo() failed"));
        return false;
    }
    TraceMessage(__FILE__, __LINE__, __func__, "Client SendTo sent={}", numBytesSent);
    sockaddr_in peerAddress {};
    socklen_t peerAddressSize = sizeof(peerAddress);
    int numBytesReceived = api.ReceiveFrom(handle, bufferIn, Size, 0, reinterpret_cast<sockaddr *>(&peerAddress), &peerAddressSize);
    if (numBytesReceived == -1)
    {
        int errorCode = GetError();
        tracing::Logging::Error(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "Client ReceiveFrom() failed"));
        return false;
    }
    TraceMessage(__FILE__, __LINE__, __func__, "Client ReceiveFrom received={}", numBytesReceived);
    if (Size != numBytesSent)
        return false;
    if (Size != numBytesReceived)
        return false;
    if (!std::equal(std::begin(bufferIn), std::end(bufferIn), std::begin(bufferOut)))
        return false;
    return true;
}

bool ExchangeDataConnected(SocketAPI & api, SocketHandle handle)
{
    const int Size = 10;
    std::uint8_t bufferOut[Size] = { 'H', 'e', 'l', 'l', 'o', 'W', 'o', 'r', 'l', 'd'};
    std::uint8_t bufferIn[Size];
    int numBytesSent = api.Send(handle, bufferOut, Size, 0);
    if (numBytesSent == -1)
    {
        int errorCode = GetError();
        tracing::Logging::Error(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "Send() failed"));
        return false;
    }
    TraceMessage(__FILE__, __LINE__, __func__, "Send sent={}", numBytesSent);
    int numBytesReceived = api.Receive(handle, bufferIn, static_cast<std::size_t>(numBytesSent), 0);
    if (numBytesReceived == -1)
    {
        int errorCode = GetError();
        tracing::Logging::Error(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "Receive() failed"));
        return false;
    }
    TraceMessage(__FILE__, __LINE__, __func__, "Receive received={}", numBytesReceived);
    if (Size != numBytesSent)
        return false;
    if (Size != numBytesReceived)
        return false;
    if (!std::equal(std::begin(bufferIn), std::end(bufferIn), std::begin(bufferOut)))
        return false;
    return true;
}

bool Connect(SocketAPI & api, SocketHandle handle, const sockaddr_in & address)
{
    bool connected {};
    SocketTimeout timeout = 5000;
    int result {};
    TraceMessage(__FILE__, __LINE__, __func__, "Client Connect start");
#if defined(PLATFORM_LINUX)
    result = api.Connect(handle, reinterpret_cast<const sockaddr *>(&address), sizeof(address));
    if (result == -1)
    {
        int errorCode = GetError();
        tracing::Logging::Error(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "Client Connect() error"));

        if ((errorCode == EINPROGRESS) || (errorCode == EALREADY))
        {
            pollfd descriptor;
            descriptor.fd = handle;
            descriptor.events = POLLIN | POLLOUT;
            descriptor.revents = 0;
            int pollResult = ::poll(&descriptor, 1, timeout);
            if (pollResult == -1)
            {
                errorCode = GetError();
            }
            else if (pollResult == 0)
            {
            }
            else
            {
                if (!(descriptor.revents & POLLHUP))
                    result = 0;
            }
        }
        else if ((errorCode != EWOULDBLOCK) && (errorCode != EAGAIN))
            tracing::Logging::Error(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "Client Connect() failed"));
    }
#elif defined(PLATFORM_WINDOWS)
    SocketTimeout waitTime = timeout;
    result = api.Connect(handle, reinterpret_cast<const sockaddr *>(&address), sizeof(address));
    if (result == -1)
    {
        int errorCode = GetError();
        tracing::Logging::Error(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "Client Connect() error"));

        while ((waitTime > 0) && ((errorCode == EWOULDBLOCK) || (errorCode == EINPROGRESS)))
        {
            timeval waitInterval { 0, 1000 * TimeWait };
            fd_set readfds;
            fd_set writefds {1, {handle}};
            fd_set exceptfds {1, {handle}};
            FD_ZERO(&readfds);
            FD_ZERO(&writefds);
            FD_ZERO(&exceptfds);
            FD_SET(handle, &writefds);
            FD_SET(handle, &exceptfds);
            tracing::Tracing::Trace(tracing::TraceCategory::Data, __FILE__, __LINE__, __func__, "connect() wait, {} ms left", waitTime);
            int selectResult = select(1, &readfds, &writefds, &exceptfds, &waitInterval);
            TraceMessage(__FILE__, __LINE__, __func__, "select() result={}", selectResult);
            waitTime -= std::min(waitTime, TimeWait);
            if (selectResult == -1)
            {
                errorCode = GetError();

                tracing::Logging::Error(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "select() failed"));
                break;
            }
            else if (selectResult == 0)
            {
                // errorCode = 0;
                // tracing::Logging::Error(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "connect() timeout"));
                // break;
            }
            if (FD_ISSET(handle, &exceptfds))
            {
                TraceMessage(__FILE__, __LINE__, __func__, "Client Connect() exception");
            }
            if (FD_ISSET(handle, &writefds))
            {
                TraceMessage(__FILE__, __LINE__, __func__, "Client Connect() success");
                result = 0;
                errorCode = 0;
                break;
            }
        }
        if (result == -1)
            tracing::Logging::Error(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "Client Connect() timeout"));
    }
#endif
    // Note: UDP connection will always succeed, as it simply registers the address
    TraceMessage(__FILE__, __LINE__, __func__, "Client Connect end result={}", result);
    if (result != -1)
    {
#if defined(__FreeBSD__) || defined(BSD)
        // Special case for FreeBSD7, for which send() doesn't do the trick
        struct sockaddr_in junk;
        socklen_t length = sizeof(junk);
        memset(&junk, 0, sizeof(junk));
        result = api.GetRemoteAddress(handle, reinterpret_cast<struct sockaddr *>(&junk), &length);
#else
        // For most platforms, the code below is all we need
        //std::uint8_t junk;
        //result = api.Send(handle, &junk, 0, 0);
#endif
    }
    connected = (result != -1);
    TraceMessage(__FILE__, __LINE__, __func__, "Client Connect end connected={}", connected);
    return connected;
}

bool ConnectNonBlocking(SocketAPI & api, SocketHandle handle, const sockaddr_in & address)
{
    if (!SetBlockingMode(api, handle, false))
        return false;

    bool connected = Connect(api, handle, address);
    if (!SetBlockingMode(api, handle, true))
        return false;
    return connected;
}

bool Bind(SocketAPI & api, SocketHandle handle, const sockaddr_in & address)
{
    TraceMessage(__FILE__, __LINE__, __func__, "Bind");
    int result = api.Bind(handle, reinterpret_cast<const sockaddr *>(&address), sizeof(address));
    if (result == -1)
    {
        int errorCode = GetError();
        tracing::Logging::Error(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "Bind() failed"));
        return false;
    }
    return true;
}

bool Listen(SocketAPI & api, SocketHandle handle, int numListeners)
{
    TraceMessage(__FILE__, __LINE__, __func__, "Server Listen");
    int result = api.Listen(handle, numListeners);
    if (result == -1)
    {
        int errorCode = GetError();
        tracing::Logging::Error(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "Server Listen() failed"));
        return false;
    }
    return true;
}

bool SocketAPITest_TCPAcceptFunction()
{
    SocketAPI api;
    SocketHandle acceptorHandle = api.Open(SocketFamily::InternetV4, SocketType::Stream, SocketProtocol::IP);
    sockaddr_in serverAddress {};
    FillAddress(serverAddress, TestPort, IPV4Address::LocalHost.GetUInt32());

    if (!Bind(api, acceptorHandle, serverAddress))
    {
        api.Close(acceptorHandle);
        return false;
    }

    if (!Listen(api, acceptorHandle, 1))
    {
        api.Close(acceptorHandle);
        return false;
    }

    if (!SetBlockingMode(api, acceptorHandle, false))
    {
        api.Close(acceptorHandle);
        return false;
    }

    SocketHandle clientHandle;
    sockaddr_in clientAddress {};
    socklen_t clientAddressSize = sizeof(clientAddress);

    SocketTimeout timeout = 5000;
    SocketTimeout waitTime = timeout;

    TraceMessage(__FILE__, __LINE__, __func__, "Server Accept start");

    do
    {
        clientHandle = static_cast<SocketHandle>(api.Accept(acceptorHandle, reinterpret_cast<sockaddr *>(&clientAddress), &clientAddressSize));
        if (clientHandle == -1)
        {
            int errorCode = GetError();

            tracing::Logging::Error(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "Server Accept() error"));

            if (((errorCode == EWOULDBLOCK) || (errorCode == EAGAIN)) && (waitTime > 0))
            {
                TraceMessage(__FILE__, __LINE__, __func__, "Server Accept() wait, {} ms left", waitTime);
                std::this_thread::sleep_for(std::chrono::milliseconds(TimeWait));
                waitTime -= std::min(waitTime, TimeWait);
            }
            else if (errorCode == EBADF)
            {
                break;
            }
            else
            {
                tracing::Logging::Error(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "Server Accept() failed"));
                break;
            }
        }
    }
    while ((clientHandle == -1) && (waitTime > 0));
    bool accepted = (clientHandle != -1);

    TraceMessage(__FILE__, __LINE__, __func__, "Server Accept end");

    if (!SetBlockingMode(api, acceptorHandle, true))
    {
        api.Close(acceptorHandle);
        return false;
    }

    if (accepted)
    {
        const int Size = 10;
        std::uint8_t buffer[Size];
        TraceMessage(__FILE__, __LINE__, __func__, "Client set blocking mode");
        if (!SetBlockingMode(api, clientHandle, true))
        {
            api.Close(clientHandle);
            api.Close(acceptorHandle);
            return false;
        }

        int numBytesReceived = api.Receive(clientHandle, buffer, Size, 0);
        if (numBytesReceived == -1)
        {
            int errorCode = GetError();
            tracing::Logging::Error(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "Server Receive() failed"));
            api.Close(clientHandle);
            api.Close(acceptorHandle);
            return false;
        }
        TraceMessage(__FILE__, __LINE__, __func__, "Server Receive received={}", numBytesReceived);
        int numBytesSent = api.Send(clientHandle, buffer, static_cast<std::size_t>(numBytesReceived), 0);
        if (numBytesSent == -1)
        {
            int errorCode = GetError();
            tracing::Logging::Error(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "Server Send() failed"));
            api.Close(clientHandle);
            api.Close(acceptorHandle);
            return false;
        }
        TraceMessage(__FILE__, __LINE__, __func__, "Server Send sent={}", numBytesSent);
    }
    // Wait for client to close connection, otherwise we'll end up in TIME_WAIT status
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    TraceMessage(__FILE__, __LINE__, __func__, "Server close down");
    api.Close(clientHandle);
    api.Close(acceptorHandle);

    return accepted;
}

bool SocketAPITest_TCPClientFunction()
{
    SocketAPI api;
    SocketHandle clientHandle = api.Open(SocketFamily::InternetV4, SocketType::Stream, SocketProtocol::IP);
    if (clientHandle == InvalidHandleValue)
        return false;
    sockaddr_in serverAddress {};
    FillAddress(serverAddress, TestPort, IPV4Address::LocalHost.GetUInt32());

    bool connected = ConnectNonBlocking(api, clientHandle, serverAddress);
    if (connected)
    {
        if (!ExchangeDataConnected(api, clientHandle))
        {
            api.Close(clientHandle);
            return false;
        }
    }
    // Make sure to close client before server ends, otherwise we'll end up in TIME_WAIT status
    TraceMessage(__FILE__, __LINE__, __func__, "Client close down");
    api.Close(clientHandle);

    return connected;
}

bool SocketAPITest_UDPServerFunction()
{
    SocketAPI api;
    SocketHandle serverHandle = api.Open(SocketFamily::InternetV4, SocketType::Datagram, SocketProtocol::IP);
    sockaddr_in serverAddress {};
    FillAddress(serverAddress, TestPort, IPV4Address::Any.GetUInt32());

    if (!Bind(api, serverHandle, serverAddress))
    {
        api.Close(serverHandle);
        return false;
    }
    const std::size_t Size = 10;
    std::uint8_t buffer[Size];
    sockaddr_in clientAddress {};
    socklen_t clientAddressSize = sizeof(clientAddress);
    int numBytesReceived = api.ReceiveFrom(serverHandle, buffer, Size, 0, reinterpret_cast<sockaddr *>(&clientAddress), &clientAddressSize);
    if (numBytesReceived == -1)
    {
        int errorCode = GetError();
        tracing::Logging::Error(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "Server ReceiveFrom() failed"));
        api.Close(serverHandle);
        return false;
    }
    TraceMessage(__FILE__, __LINE__, __func__, "Server ReceiveFrom received={}", numBytesReceived);
    int numBytesSent = api.SendTo(serverHandle, buffer, static_cast<std::size_t>(numBytesReceived), 0, reinterpret_cast<const sockaddr *>(&clientAddress), clientAddressSize);
    if (numBytesSent == -1)
    {
        int errorCode = GetError();
        tracing::Logging::Error(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "Server SendTo() failed"));
        api.Close(serverHandle);
        return false;
    }
    TraceMessage(__FILE__, __LINE__, __func__, "Server SendTo sent={}", numBytesSent);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    TraceMessage(__FILE__, __LINE__, __func__, "Server close down");
    api.Close(serverHandle);

    return numBytesSent == Size;
}

bool SocketAPITest_UDPClientFunction()
{
    SocketAPI api;
    SocketHandle clientHandle = api.Open(SocketFamily::InternetV4, SocketType::Datagram, SocketProtocol::IP);
    if (clientHandle == InvalidHandleValue)
        return false;
    if (!ExchangeDataNonConnected(api, clientHandle))
    {
        api.Close(clientHandle);
        return false;
    }
    TraceMessage(__FILE__, __LINE__, __func__, "Client close down");
    api.Close(clientHandle);

    return true;
}

bool SocketAPITest_UDPConnectedClientFunction()
{
    SocketAPI api;
    SocketHandle clientHandle = api.Open(SocketFamily::InternetV4, SocketType::Datagram, SocketProtocol::IP);
    if (clientHandle == InvalidHandleValue)
        return false;
    sockaddr_in serverAddress {};
    FillAddress(serverAddress, TestPort, IPV4Address::LocalHost.GetUInt32());

    // Connect always succeeds whether blocking or non-blocking and returns immediately, as only the server address is recorded
    // As the non-blocking connect performs a send of 0 bytes, the recv will receive 0 bytes, so we choose to connect blocking
    bool connected = Connect(api, clientHandle, serverAddress);
    if (connected)
    {
        if (!ExchangeDataConnected(api, clientHandle))
        {
            api.Close(clientHandle);
            return false;
        }
    }

    // Make sure to close client before server ends, otherwise we'll end up in TIME_WAIT status
    TraceMessage(__FILE__, __LINE__, __func__, "Client close down");
    api.Close(clientHandle);

    return true;
}

TEST_F(SocketAPITest, SendReceiveUDPConnectionlessSucceeds)
{
    core::TypedReturnThread<bool> serverThread(SocketAPITest_UDPServerFunction);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    bool connectionOk = SocketAPITest_UDPClientFunction();
    EXPECT_TRUE(connectionOk);
    serverThread.WaitForDeath();
    bool success = serverThread.GetResult();
    EXPECT_TRUE(success);
}

TEST_F(SocketAPITest, SendReceiveUDPConnectionlessFailsIfNoPeer)
{
    bool connectionOk = SocketAPITest_UDPClientFunction();
    EXPECT_FALSE(connectionOk);
}

TEST_F(SocketAPITest, SendReceiveUDPConnectedSucceeds)
{
    core::TypedReturnThread<bool> serverThread(SocketAPITest_UDPServerFunction);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    bool connectionOk = SocketAPITest_UDPConnectedClientFunction();
    EXPECT_TRUE(connectionOk);
    serverThread.WaitForDeath();
    bool success = serverThread.GetResult();
    EXPECT_TRUE(success);
}

TEST_F(SocketAPITest, SendReceiveUDPConnectedFailsIfNoServer)
{
    bool connectionOk = SocketAPITest_UDPConnectedClientFunction();
    EXPECT_FALSE(connectionOk);
}

TEST_F(SocketAPITest, ConnectAcceptSendReceiveTCPSucceeds)
{
    core::TypedReturnThread<bool> acceptorThread(SocketAPITest_TCPAcceptFunction);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    bool connectionOk = SocketAPITest_TCPClientFunction();
    EXPECT_TRUE(connectionOk);
    acceptorThread.WaitForDeath();
    bool accepted = acceptorThread.GetResult();
    EXPECT_TRUE(accepted);
}

TEST_F(SocketAPITest, ConnectAcceptSendReceiveTCPFailsIfNoServer)
{
    bool connectionOk = SocketAPITest_TCPClientFunction();
    EXPECT_FALSE(connectionOk);
}

TEST_F(SocketAPITest, ConnectAcceptSendReceiveTCPFailsAfterFirstSuccessfulConnectWhenServerNoLongerAccepting)
{
    core::TypedReturnThread<bool> acceptorThread(SocketAPITest_TCPAcceptFunction);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    SocketHandle clientHandle1 = api.Open(SocketFamily::InternetV4, SocketType::Stream, SocketProtocol::IP);
    EXPECT_NE(InvalidHandleValue, clientHandle1);
    SocketHandle clientHandle2 = api.Open(SocketFamily::InternetV4, SocketType::Stream, SocketProtocol::IP);
    EXPECT_NE(InvalidHandleValue, clientHandle2);
    sockaddr_in serverAddress {};
    FillAddress(serverAddress, TestPort, IPV4Address::LocalHost.GetUInt32());

    bool connected1 = ConnectNonBlocking(api, clientHandle1, serverAddress);
    bool connected2 = ConnectNonBlocking(api, clientHandle2, serverAddress);
    EXPECT_TRUE(connected1);
    // Weirdly, another connection to the same server succeeds even though it is not accepting
    EXPECT_TRUE(connected2);
    if (connected1)
    {
        EXPECT_TRUE(ExchangeDataConnected(api, clientHandle1));
    }
    if (connected2)
    {
        // However once we do a receive, it fails
        EXPECT_FALSE(ExchangeDataConnected(api, clientHandle2));
    }
    api.Close(clientHandle1);
    api.Close(clientHandle2);

    acceptorThread.WaitForDeath();
    bool accepted = acceptorThread.GetResult();
    EXPECT_TRUE(accepted);
}

} // namespace testing
} // namespace network
