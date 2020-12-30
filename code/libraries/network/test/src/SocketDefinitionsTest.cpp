#include "GoogleTest.h"

#include "network/SocketDefinitions.h"

namespace network {

static std::string Serialize(SocketFamily value)
{
    std::ostringstream stream;
    stream << value;
    return stream.str();
}
static std::string Serialize(SocketType value)
{
    std::ostringstream stream;
    stream << value;
    return stream.str();
}
static std::string Serialize(SocketOptionLevel value)
{
    std::ostringstream stream;
    stream << value;
    return stream.str();
}
static std::string Serialize(SocketOption value)
{
    std::ostringstream stream;
    stream << value;
    return stream.str();
}

TEST(NetworkTest, SerializeSocketFamily)
{
    EXPECT_EQ("Any", Serialize(SocketFamily::Any));
    EXPECT_EQ("Unix", Serialize(SocketFamily::Unix));
    EXPECT_EQ("InternetV4", Serialize(SocketFamily::InternetV4));
    EXPECT_EQ("InternetV6", Serialize(SocketFamily::InternetV6));
    EXPECT_EQ("IPX", Serialize(SocketFamily::IPX));
#if defined(PLATFORM_LINUX)
    EXPECT_EQ("Packet", Serialize(SocketFamily::Packet));    
#endif
}

TEST(NetworkTest, SerializeSocketType)
{
    EXPECT_EQ("Stream", Serialize(SocketType::Stream));
    EXPECT_EQ("Datagram", Serialize(SocketType::Datagram));
    EXPECT_EQ("Raw", Serialize(SocketType::Raw));
    EXPECT_EQ("SeqPacket", Serialize(SocketType::SeqPacket));
}

TEST(NetworkTest, SerializeSocketOptionLevel)
{
    EXPECT_EQ("Socket", Serialize(SocketOptionLevel::Socket));
}

TEST(NetworkTest, SerializeSocketOption)
{
    EXPECT_EQ("Debug", Serialize(SocketOption::Debug));
    EXPECT_EQ("ReuseAddress", Serialize(SocketOption::ReuseAddress));
    EXPECT_EQ("Type", Serialize(SocketOption::Type));
    EXPECT_EQ("Error", Serialize(SocketOption::Error));
    EXPECT_EQ("DontRoute", Serialize(SocketOption::DontRoute));
    EXPECT_EQ("Broadcast", Serialize(SocketOption::Broadcast));
    EXPECT_EQ("SendBuffer", Serialize(SocketOption::SendBuffer));
    EXPECT_EQ("ReceiveBuffer", Serialize(SocketOption::ReceiveBuffer));
#if defined(PLATFORM_LINUX)
    EXPECT_EQ("SendBufferForce", Serialize(SocketOption::SendBufferForce));
    EXPECT_EQ("ReceiveBufferForce", Serialize(SocketOption::ReceiveBufferForce));
#endif
    EXPECT_EQ("KeepAlive", Serialize(SocketOption::KeepAlive));
    EXPECT_EQ("OutOfBandInline", Serialize(SocketOption::OutOfBandInline));
#if defined(PLATFORM_LINUX)
    EXPECT_EQ("NoCheck", Serialize(SocketOption::NoCheck));
    EXPECT_EQ("Priority", Serialize(SocketOption::Priority));
#endif
    EXPECT_EQ("Linger", Serialize(SocketOption::Linger));
#if defined(PLATFORM_LINUX)
    EXPECT_EQ("BSDCompatibility", Serialize(SocketOption::BSDCompatibility));
#endif
    EXPECT_EQ("ReceiveTimeout", Serialize(SocketOption::ReceiveTimeout));
    EXPECT_EQ("SendTimeout", Serialize(SocketOption::SendTimeout));
#if defined(PLATFORM_LINUX)
    EXPECT_EQ("BindToDevice", Serialize(SocketOption::BindToDevice));
    EXPECT_EQ("Timestamping", Serialize(SocketOption::Timestamping));
#endif
}

} // namespace network