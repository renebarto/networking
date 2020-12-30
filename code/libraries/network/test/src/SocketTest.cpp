#include "GoogleTest.h"

#include "network/Socket.h"

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

} // namespace network