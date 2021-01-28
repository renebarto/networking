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

bool SocketAPITest_TCPAcceptFunction()
{
    SocketAPI api;
    SocketHandle acceptorHandle = api.Open(SocketFamily::InternetV4, SocketType::Stream, SocketProtocol::IP);
    sockaddr_in serverAddress {};
    FillAddress(serverAddress, TestPort, IPV4Address::LocalHost.GetUInt32());

    TraceMessage(__FILE__, __LINE__, __func__, "Server Bind");
    int result = api.Bind(acceptorHandle, reinterpret_cast<sockaddr *>(&serverAddress), sizeof(serverAddress));
    if (result == -1)
    {
        int errorCode = GetError();
        tracing::Logging::Error(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "Server Bind() failed"));
        return false;
    }
    TraceMessage(__FILE__, __LINE__, __func__, "Server Listen");
    result = api.Listen(acceptorHandle, 1);
    if (result == -1)
    {
        int errorCode = GetError();
        tracing::Logging::Error(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "Server Listen() failed"));
        return false;
    }
    TraceMessage(__FILE__, __LINE__, __func__, "Server Set non blocking mode");
    if (-1 == api.SetBlockingMode(acceptorHandle, false))
    {
        int errorCode = GetError();
        tracing::Logging::Error(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "Server SetBlockingMode() failed"));
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

    TraceMessage(__FILE__, __LINE__, __func__, "Server set blocking mode");
    if (-1 == api.SetBlockingMode(acceptorHandle, true))
    {
        int errorCode = GetError();
        tracing::Logging::Error(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "Server SetBlockingMode() failed"));
        return false;
    }

    if (accepted)
    {
        const int Size = 10;
        std::uint8_t buffer[Size];
        TraceMessage(__FILE__, __LINE__, __func__, "Client set blocking mode");
        if (-1 == api.SetBlockingMode(clientHandle, true))
        {
            int errorCode = GetError();
            tracing::Logging::Error(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "Client SetBlockingMode() failed"));
            return false;
        }

        int numBytesReceived = api.Receive(clientHandle, buffer, Size, 0);
        if (numBytesReceived == -1)
        {
            int errorCode = GetError();
            tracing::Logging::Error(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "Server Receive() failed"));
        }
        TraceMessage(__FILE__, __LINE__, __func__, "Server Receive received={}", numBytesReceived);
        int numBytesSent = api.Send(clientHandle, buffer, static_cast<std::size_t>(numBytesReceived), 0);
        if (numBytesSent == -1)
        {
            int errorCode = GetError();
            tracing::Logging::Error(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "Server Send() failed"));
        }
        TraceMessage(__FILE__, __LINE__, __func__, "Server Send sent={}", numBytesSent);
    }
    // Wait for client to close connection, otherwise we'll end up in TIME_WAIT status
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
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

    TraceMessage(__FILE__, __LINE__, __func__, "Client Set non blocking mode");
    if (-1 == api.SetBlockingMode(clientHandle, false))
    {
        int errorCode = GetError();
        tracing::Logging::Error(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "Client SetBlockingMode() failed"));
        return false;
    }
    bool connected {};
    SocketTimeout timeout = 5000;
    SocketTimeout waitTime = timeout;
    int result {};
    TraceMessage(__FILE__, __LINE__, __func__, "Client Connect start");
#if defined(PLATFORM_LINUX)
    result = api.Connect(clientHandle, serverAddress, serverAddressLength);
    if (result == -1)
    {
        int errorCode = GetError();
        tracing::Logging::Error(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "Client Connect() error"));

        if ((errorCode == EINPROGRESS) || (errorCode == EALREADY))
        {
            pollfd descriptor;
            descriptor.fd = clientHandle;
            descriptor.events = POLLIN | POLLOUT;
            descriptor.revents = 0;
            int pollResult = ::poll(&descriptor, 1, timeout);
            if (pollResult == -1)
            {
                errorCode = GetError();
            }
            else if (pollResult == 0)
            {
                return false;
            }
            else
            {
                if ((descriptor.revents & POLLHUP))
                    return false;
                result = 0;
            }
        }
        else if ((errorCode != EWOULDBLOCK) && (errorCode != EAGAIN))
            tracing::Logging::Error(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "Client Connect() failed"));
    }
#elif defined(PLATFORM_WINDOWS)
    result = api.Connect(clientHandle, reinterpret_cast<sockaddr *>(&serverAddress), sizeof(serverAddress));
    if (result == -1)
    {
        int errorCode = GetError();
        tracing::Logging::Error(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "Client Connect() error"));

        while ((waitTime > 0) && ((errorCode == EWOULDBLOCK) || (errorCode == EINPROGRESS)))
        {
            timeval waitInterval { 0, 1000 * TimeWait };
            fd_set readfds;
            fd_set writefds {1, {clientHandle}};
            fd_set exceptfds {1, {clientHandle}};
            FD_ZERO(&readfds);
            FD_ZERO(&writefds);
            FD_ZERO(&exceptfds);
            FD_SET(clientHandle, &writefds);
            FD_SET(clientHandle, &exceptfds);
            // tracing::Tracing::Trace(tracing::TraceCategory::Data, __FILE__, __LINE__, __func__, "connect() wait, {} ms left", waitTime);
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
            if (FD_ISSET(clientHandle, &exceptfds))
            {
                TraceMessage(__FILE__, __LINE__, __func__, "Client Connect() exception");
            }
            if (FD_ISSET(clientHandle, &writefds))
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
    TraceMessage(__FILE__, __LINE__, __func__, "Client Connect end");
    connected = (result != -1);
    TraceMessage(__FILE__, __LINE__, __func__, "Client Set blocking mode");
    if (-1 == api.SetBlockingMode(clientHandle, true))
    {
        int errorCode = GetError();
        tracing::Logging::Error(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "Client SetBlockingMode() failed"));
        return false;
    }
    if (!connected)
        return false;

    const int Size = 10;
    std::uint8_t bufferOut[Size] = { 'H', 'e', 'l', 'l', 'o', 'W', 'o', 'r', 'l', 'd'};
    std::uint8_t bufferIn[Size];
    int numBytesSent = api.Send(clientHandle, bufferOut, Size, 0);
    if (numBytesSent == -1)
    {
        int errorCode = GetError();
        tracing::Logging::Error(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "Client Send() failed"));
        return false;
    }
    TraceMessage(__FILE__, __LINE__, __func__, "Client Send sent={}", numBytesSent);
    int numBytesReceived = api.Receive(clientHandle, bufferIn, static_cast<std::size_t>(numBytesSent), 0);
    if (numBytesReceived == -1)
    {
        int errorCode = GetError();
        tracing::Logging::Error(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "Client Receive() failed"));
        return false;
    }
    TraceMessage(__FILE__, __LINE__, __func__, "Client Receive received={}", numBytesReceived);
    if (Size != numBytesSent)
        return false;
    if (Size != numBytesReceived)
        return false;
    if (!std::equal(std::begin(bufferIn), std::end(bufferIn), std::begin(bufferOut)))
        return false;
    // Make sure to close client before server ends, otherwise we'll end up in TIME_WAIT status
    TraceMessage(__FILE__, __LINE__, __func__, "Client close down");
    api.Close(clientHandle);

    return true;
}

bool SocketAPITest_UDPServerFunction()
{
    SocketAPI api;
    SocketHandle serverHandle = api.Open(SocketFamily::InternetV4, SocketType::Datagram, SocketProtocol::IP);
    sockaddr_in serverAddress {};
    FillAddress(serverAddress, TestPort, IPV4Address::Any.GetUInt32());

    TraceMessage(__FILE__, __LINE__, __func__, "Server Bind");
    int result = api.Bind(serverHandle, reinterpret_cast<sockaddr *>(&serverAddress), sizeof(serverAddress));
    if (result == -1)
    {
        int errorCode = GetError();
        tracing::Logging::Error(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "Server Bind() failed"));
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
        return false;
    }
    TraceMessage(__FILE__, __LINE__, __func__, "Server ReceiveFrom received={}", numBytesReceived);
    int numBytesSent = api.SendTo(serverHandle, buffer, static_cast<std::size_t>(numBytesReceived), 0, reinterpret_cast<const sockaddr *>(&clientAddress), clientAddressSize);
    if (numBytesSent == -1)
    {
        int errorCode = GetError();
        tracing::Logging::Error(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "Server SendTo() failed"));
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

    sockaddr_in serverAddress {};
    FillAddress(serverAddress, TestPort, IPV4Address::LocalHost.GetUInt32());

    const int Size = 10;
    std::uint8_t bufferOut[Size] = { 'H', 'e', 'l', 'l', 'o', 'W', 'o', 'r', 'l', 'd'};
    std::uint8_t bufferIn[Size];
    int numBytesSent = api.SendTo(clientHandle, bufferOut, Size, 0, reinterpret_cast<const sockaddr *>(&serverAddress), sizeof(serverAddress));
    if (numBytesSent == -1)
    {
        int errorCode = GetError();
        tracing::Logging::Error(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "Client SendTo() failed"));
        return false;
    }
    TraceMessage(__FILE__, __LINE__, __func__, "Client SendTo sent={}", numBytesSent);
    sockaddr_in peerAddress {};
    socklen_t peerAddressSize = sizeof(peerAddress);
    int numBytesReceived = api.ReceiveFrom(clientHandle, bufferIn, Size, 0, reinterpret_cast<sockaddr *>(&peerAddress), &peerAddressSize);
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

    TraceMessage(__FILE__, __LINE__, __func__, "Client Set non blocking mode");
    if (-1 == api.SetBlockingMode(clientHandle, false))
    {
        int errorCode = GetError();
        tracing::Logging::Error(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "Client SetBlockingMode() failed"));
        return false;
    }
    bool connected {};
    SocketTimeout timeout = 5000;
    SocketTimeout waitTime = timeout;
    int result {};
    TraceMessage(__FILE__, __LINE__, __func__, "Client Connect start");
#if defined(PLATFORM_LINUX)
    result = api.Connect(clientHandle, serverAddress, serverAddressLength);
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
                return false;
            }
            else
            {
                if ((descriptor.revents & POLLHUP))
                    return false;
                result = 0;
            }
        }
        else if ((errorCode != EWOULDBLOCK) && (errorCode != EAGAIN))
            tracing::Logging::Error(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "Client Connect() failed"));
    }
#elif defined(PLATFORM_WINDOWS)
    result = api.Connect(clientHandle, reinterpret_cast<sockaddr *>(&serverAddress), sizeof(serverAddress));
    if (result == -1)
    {
        int errorCode = GetError();
        tracing::Logging::Error(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "Client Connect() error"));

        while ((waitTime > 0) && ((errorCode == EWOULDBLOCK) || (errorCode == EINPROGRESS)))
        {
            timeval waitInterval { 0, 1000 * TimeWait };
            fd_set readfds;
            fd_set writefds {1, {clientHandle}};
            fd_set exceptfds {1, {clientHandle}};
            FD_ZERO(&readfds);
            FD_ZERO(&writefds);
            FD_ZERO(&exceptfds);
            FD_SET(clientHandle, &writefds);
            FD_SET(clientHandle, &exceptfds);
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
            if (FD_ISSET(clientHandle, &exceptfds))
            {
                TraceMessage(__FILE__, __LINE__, __func__, "Client Connect() exception");
            }
            if (FD_ISSET(clientHandle, &writefds))
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
    TraceMessage(__FILE__, __LINE__, __func__, "Client Connect end");
    connected = (result != -1);
    TraceMessage(__FILE__, __LINE__, __func__, "Client Set blocking mode");
    if (-1 == api.SetBlockingMode(clientHandle, true))
    {
        int errorCode = GetError();
        tracing::Logging::Error(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "Client SetBlockingMode() failed"));
        return false;
    }
    if (!connected)
        return false;
    if (connected)
    {
        const int Size = 10;
        std::uint8_t bufferOut[Size] = { 'H', 'e', 'l', 'l', 'o', 'W', 'o', 'r', 'l', 'd'};
        std::uint8_t bufferIn[Size];
        int numBytesSent = api.Send(clientHandle, bufferOut, Size, 0);
        if (numBytesSent == -1)
        {
            int errorCode = GetError();
            tracing::Logging::Error(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "Client Send() failed"));
            return false;
        }
        TraceMessage(__FILE__, __LINE__, __func__, "Client Send sent={}", numBytesSent);
        int numBytesReceived = api.Receive(clientHandle, bufferIn, static_cast<std::size_t>(numBytesSent), 0);
        if (numBytesReceived == -1)
        {
            int errorCode = GetError();
            tracing::Logging::Error(__FILE__, __LINE__, __func__, utility::Error(errorCode, GetErrorString(errorCode), "Client Receive() failed"));
            return false;
        }
        TraceMessage(__FILE__, __LINE__, __func__, "Client Receive received={}", numBytesReceived);
        if (Size != numBytesSent)
            return false;
        if (Size != numBytesReceived)
            return false;

        if (!std::equal(std::begin(bufferIn), std::end(bufferIn), std::begin(bufferOut)))
            return false;
        // Make sure to close client before server ends, otherwise we'll end up in TIME_WAIT status
        TraceMessage(__FILE__, __LINE__, __func__, "Client close down");
        api.Close(clientHandle);
    }
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

} // namespace testing
} // namespace network
