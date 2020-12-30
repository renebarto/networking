#pragma once

#include <array>
#include <cstdint>

#if defined(PLATFORM_LINUX) || defined(PLATFORM_LINUX_RPI)

#include <strings.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <net/if.h>
#include <ifaddrs.h>
#include <sys/ioctl.h>
#include <unistd.h>

#elif defined(PLATFORM_WINDOWS)

#pragma warning(disable: 5039)
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma warning(default: 5039)

#else

#error Unsupported platform

#endif

namespace network {

#if defined(PLATFORM_LINUX) || defined(PLATFORM_LINUX_RPI)

using SocketTimeout = int;
using SocketHandle = int;

enum class SocketFamily
{
    Any = AF_UNSPEC,
    Unix = AF_UNIX,
    Internet = AF_INET,
    InternetV4 = AF_INET,
    InternetV6 = AF_INET6,
    IPX = AF_IPX,
    Packet = AF_PACKET,
};

enum class SocketType
{
    Stream = SOCK_STREAM,
    Datagram = SOCK_DGRAM,
    Raw = SOCK_RAW,
    SeqPacket = SOCK_SEQPACKET,
};

enum class SocketOptionLevel
{
    Socket = SOL_SOCKET,
};

enum class SocketOption
{
    Debug = SO_DEBUG,
    ReuseAddress = SO_REUSEADDR,
    Type = SO_TYPE,
    Error = SO_ERROR,
    DontRoute = SO_DONTROUTE,
    Broadcast = SO_BROADCAST,
    SendBuffer = SO_SNDBUF,
    ReceiveBuffer = SO_RCVBUF,
    SendBufferForce = SO_SNDBUFFORCE,
    ReceiveBufferForce = SO_RCVBUFFORCE,
    KeepAlive = SO_KEEPALIVE,
    OutOfBandInline = SO_OOBINLINE,
    NoCheck = SO_NO_CHECK,
    Priority = SO_PRIORITY,
    Linger = SO_LINGER,
    BSDCompatibility = SO_BSDCOMPAT,
    ReceiveTimeout = SO_RCVTIMEO,
    SendTimeout = SO_SNDTIMEO,
    BindToDevice = SO_BINDTODEVICE,
    Timestamping = SO_TIMESTAMPING
};

#elif defined(PLATFORM_WINDOWS)

using SocketTimeout = int;
using SocketHandle = SOCKET;

num class SocketFamily
{
    Any = AF_UNSPEC,
    Unix = AF_UNIX,
    Internet = AF_INET,
    InternetV4 = AF_INET,
    InternetV6 = AF_INET6,
    IPX = AF_IPX,
};

enum class SocketType
{
    Stream = SOCK_STREAM,
    Datagram = SOCK_DGRAM,
    Raw = SOCK_RAW,
    SeqPacket = SOCK_SEQPACKET,
};

enum class SocketOptionLevel
{
    Socket = SOL_SOCKET,
};

enum class SocketOption
{
    Debug = SO_DEBUG,
    ReuseAddress = SO_REUSEADDR,
    Type = SO_TYPE,
    Error = SO_ERROR,
    DontRoute = SO_DONTROUTE,
    Broadcast = SO_BROADCAST,
    SendBuffer = SO_SNDBUF,
    ReceiveBuffer = SO_RCVBUF,
    KeepAlive = SO_KEEPALIVE,
    OutOfBandInline = SO_OOBINLINE,
    Linger = SO_LINGER,
    ReceiveTimeout = SO_RCVTIMEO,
    SendTimeout = SO_SNDTIMEO,
};

#endif

extern const SocketHandle InvalidHandleValue;
extern const SocketTimeout InfiniteTimeout;

using IPV4AddressType = std::uint32_t;
static constexpr std::size_t IPV6AddressSize = 16;
using IPV6AddressType = std::array<std::uint8_t, IPV6AddressSize>;

void SetIPAddress(sockaddr_in & address, const IPV4AddressType & ipAddress);
void SetIPAddress(sockaddr_in6 & address, const IPV6AddressType & ipAddress);
IPV4AddressType GetIPAddress(sockaddr_in const & address);
IPV6AddressType GetIPAddress(sockaddr_in6 const & address);
bool operator == (sockaddr_in const & lhs, sockaddr_in const & rhs);
bool operator == (sockaddr_in6 const & lhs, sockaddr_in6 const & rhs);

std::ostream & operator << (std::ostream & stream, SocketFamily value);
std::ostream & operator << (std::ostream & stream, SocketType value);
std::ostream & operator << (std::ostream & stream, SocketOptionLevel value);
std::ostream & operator << (std::ostream & stream, SocketOption value);

} // namespace network