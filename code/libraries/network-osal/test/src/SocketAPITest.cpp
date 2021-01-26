#include "GoogleTest.h"

#include "network-osal/SocketAPI.h"

#include "osal/Utilities.h"
#include "core/Thread.h"
#include "network-osal/IPV4Address.h"
#include "Utility.h"

namespace network {
namespace testing {

static void FillAddress(sockaddr_in & address, std::uint16_t port, std::uint32_t ipAddress)
{
    address.sin_family = AF_INET;
    address.sin_port = osal::SwapBytes(port);
    address.sin_addr.s_addr = ipAddress;
}

class SocketAPITest : public ::testing::Test
{
public:
    SocketAPITest()
        : handle(InvalidHandleValue)
    {}

    void SetUp() override
    {
    }
    void TearDown() override
    {
        if (handle != InvalidHandleValue)
            api.Close(handle);
        handle = InvalidHandleValue;
    }
    SocketHandle handle;
    SocketAPI api;
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

bool SocketAPITest_UDPServerThread()
{
    SocketAPI api;
    SocketHandle serverSocket = api.Open(SocketFamily::InternetV4, SocketType::Datagram, SocketProtocol::IP);
    sockaddr_in serverAddress {};
    FillAddress(serverAddress, TestPort, IPV4Address::Any.GetUInt32());

    api.Bind(serverSocket, reinterpret_cast<sockaddr *>(&serverAddress), sizeof(serverAddress));

    const std::size_t Size = 10;
    std::uint8_t buffer[Size];
    sockaddr_in clientAddress {};
    socklen_t clientAddressSize = sizeof(clientAddress);
    int numBytesReceived = api.ReceiveFrom(serverSocket, buffer, Size, 0, reinterpret_cast<sockaddr *>(&clientAddress), &clientAddressSize);
    int numBytesSent = api.SendTo(serverSocket, buffer, static_cast<std::size_t>(numBytesReceived), 0, reinterpret_cast<const sockaddr *>(&clientAddress), clientAddressSize);

    return numBytesSent == Size;
}

TEST_F(SocketAPITest, SendReceiveUDPConnectionless)
{
    handle = api.Open(SocketFamily::InternetV4, SocketType::Datagram, SocketProtocol::IP);
    sockaddr_in serverAddress {};
    FillAddress(serverAddress, TestPort, IPV4Address::LocalHost.GetUInt32());

    core::TypedReturnThread<bool> serverThread(SocketAPITest_UDPServerThread);
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    const int Size = 10;
    std::uint8_t bufferOut[Size] = { 'H', 'e', 'l', 'l', 'o', 'W', 'o', 'r', 'l', 'd'};
    std::uint8_t bufferIn[Size];
    int numBytesSent = api.SendTo(handle, bufferOut, Size, 0, reinterpret_cast<const sockaddr *>(&serverAddress), sizeof(serverAddress));
    sockaddr_in peerAddress {};
    socklen_t peerAddressSize = sizeof(peerAddress);
    int numBytesReceived = api.ReceiveFrom(handle, bufferIn, Size, 0, reinterpret_cast<sockaddr *>(&peerAddress), &peerAddressSize);
    EXPECT_EQ(Size, numBytesSent);
    EXPECT_EQ(Size, numBytesReceived);
    EXPECT_TRUE(std::equal(std::begin(bufferIn), std::end(bufferIn), std::begin(bufferOut)));
    serverThread.WaitForDeath();
    bool success = serverThread.GetResult();
    EXPECT_TRUE(success);
}

} // namespace testing
} // namespace network
