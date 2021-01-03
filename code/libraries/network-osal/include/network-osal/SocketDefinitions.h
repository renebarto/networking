#pragma once

#include <array>
#include <cstdint>

#if defined(PLATFORM_LINUX) || defined(PLATFORM_LINUX_RPI)

#include <strings.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/ip_icmp.h>
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

#include "network-osal/IPV4Address.h"
#include "network-osal/IPV6Address.h"

#undef EWOULDBLOCK
#undef EINPROGRESS
#undef EALREADY
#undef ENOTSOCK
#undef EDESTADDRREQ
#undef EMSGSIZE
#undef EPROTOTYPE
#undef ENOPROTOOPT
#undef EPROTONOSUPPORT
#undef ESOCKTNOSUPPORT
#undef EOPNOTSUPP
#undef EPFNOSUPPORT
#undef EAFNOSUPPORT
#undef EADDRINUSE
#undef EADDRNOTAVAIL
#undef ENETDOWN
#undef ENETUNREACH
#undef ENETRESET
#undef ECONNABORTED
#undef ECONNRESET
#undef ENOBUFS
#undef EISCONN
#undef ENOTCONN
#undef ESHUTDOWN
#undef ETOOMANYREFS
#undef ETIMEDOUT
#undef ECONNREFUSED
#undef ELOOP
#undef ENAMETOOLONG
#undef EHOSTDOWN
#undef EHOSTUNREACH
#undef ENOTEMPTY
#undef EPROCLIM
#undef EUSERS
#undef EDQUOT
#undef ESTALE
#undef EREMOTE
#define EWOULDBLOCK             WSAEWOULDBLOCK
#define EINPROGRESS             WSAEINPROGRESS
#define EALREADY                WSAEALREADY
#define ENOTSOCK                WSAENOTSOCK
#define EDESTADDRREQ            WSAEDESTADDRREQ
#define EMSGSIZE                WSAEMSGSIZE
#define EPROTOTYPE              WSAEPROTOTYPE
#define ENOPROTOOPT             WSAENOPROTOOPT
#define EPROTONOSUPPORT         WSAEPROTONOSUPPORT
#define ESOCKTNOSUPPORT         WSAESOCKTNOSUPPORT
#define EOPNOTSUPP              WSAEOPNOTSUPP
#define EPFNOSUPPORT            WSAEPFNOSUPPORT
#define EAFNOSUPPORT            WSAEAFNOSUPPORT
#define EADDRINUSE              WSAEADDRINUSE
#define EADDRNOTAVAIL           WSAEADDRNOTAVAIL
#define ENETDOWN                WSAENETDOWN
#define ENETUNREACH             WSAENETUNREACH
#define ENETRESET               WSAENETRESET
#define ECONNABORTED            WSAECONNABORTED
#define ECONNRESET              WSAECONNRESET
#define ENOBUFS                 WSAENOBUFS
#define EISCONN                 WSAEISCONN
#define ENOTCONN                WSAENOTCONN
#define ESHUTDOWN               WSAESHUTDOWN
#define ETOOMANYREFS            WSAETOOMANYREFS
#define ETIMEDOUT               WSAETIMEDOUT
#define ECONNREFUSED            WSAECONNREFUSED
#define ELOOP                   WSAELOOP
#define ENAMETOOLONG            WSAENAMETOOLONG
#define EHOSTDOWN               WSAEHOSTDOWN
#define EHOSTUNREACH            WSAEHOSTUNREACH
#define ENOTEMPTY               WSAENOTEMPTY
#define EPROCLIM                WSAEPROCLIM
#define EUSERS                  WSAEUSERS
#define EDQUOT                  WSAEDQUOT
#define ESTALE                  WSAESTALE
#define EREMOTE                 WSAEREMOTE

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

enum class SocketProtocol
{
    IP = IPPROTO_IP,
    ICMP = IPPROTO_ICMP,
    IGMP = IPPROTO_IGMP,
    TCP = IPPROTO_TCP,
    PUP = IPPROTO_PUP,
    UDP = IPPROTO_UDP,
    IDP = IPPROTO_IDP,
    Raw = IPPROTO_RAW,
};

enum class SocketOptionLevel
{
    IP = SOL_IP,
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

enum class IPSocketOption
{
    TTL = IP_TTL,
};

#elif defined(PLATFORM_WINDOWS)

using SocketTimeout = int;
using SocketHandle = SOCKET;

enum class SocketFamily
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

enum class SocketProtocol
{
    IP = IPPROTO_IP,
    ICMP = IPPROTO_ICMP,
    IGMP = IPPROTO_IGMP,
    TCP = IPPROTO_TCP,
    PUP = IPPROTO_PUP,
    UDP = IPPROTO_UDP,
    IDP = IPPROTO_IDP,
    ND = IPPROTO_ND,
    Raw = IPPROTO_RAW,
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

enum class IPSocketOption
{
    TTL = IP_TTL,
};

#endif

extern const SocketHandle InvalidHandleValue;
extern const SocketTimeout InfiniteTimeout;

using PortType = uint16_t;

extern const PortType AnyPort;

bool operator == (sockaddr_in const & lhs, sockaddr_in const & rhs);
bool operator == (sockaddr_in6 const & lhs, sockaddr_in6 const & rhs);

std::ostream & operator << (std::ostream & stream, SocketFamily value);
std::ostream & operator << (std::ostream & stream, SocketType value);
std::ostream & operator << (std::ostream & stream, SocketProtocol value);
std::ostream & operator << (std::ostream & stream, SocketOptionLevel value);
std::ostream & operator << (std::ostream & stream, SocketOption value);
std::ostream & operator << (std::ostream & stream, IPSocketOption value);

} // namespace network