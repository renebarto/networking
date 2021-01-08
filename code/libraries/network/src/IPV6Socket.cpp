#include "network/IPV6Socket.h"

#include "tracing/ScopedTracing.h"
#include "tracing/Tracing.h"
#include "utility/Assert.h"
#include "utility/Endian.h"
#include "utility/Error.h"

namespace serialization {

std::string Serialize(const network::IPV6Socket & value, int width)
{
    return Serialize(static_cast<int>(value.GetHandle(), width));
}

} // namespace serialization

namespace network {

static const size_t BufferSize = 4096;
static constexpr SocketFamily IPV6SocketFamily = SocketFamily::InternetV6;

IPV6Socket::IPV6Socket()
    : Socket()
{
}

IPV6Socket::IPV6Socket(SocketType socketType)
    : Socket(IPV6SocketFamily, socketType)
{
}

IPV6Socket::IPV6Socket(const IPV6Socket & other)
    : Socket(other)
{
}
IPV6Socket::IPV6Socket(IPV6Socket && other)
    : Socket(std::move(other))
{
}

IPV6Socket & IPV6Socket::operator = (IPV6Socket && other)
{
    if (this != & other)
    {
        *(Socket *)this = std::move(other);
    }
    return *this;
}

void IPV6Socket::Open(SocketType socketType, SocketProtocol protocol)
{
    SCOPEDTRACE("", [&]{
        return 
            "socketType="+ serialization::Serialize(socketType, 0) +
            " protocol="+ serialization::Serialize(protocol, 0);
    });
    Socket::Open(IPV6SocketFamily, socketType, protocol);
}
    
void IPV6Socket::Bind(const IPV6Address & ipAddress)
{
    SCOPEDTRACE("", [&]{
        return 
            "ipAddress="+ serialization::Serialize(ipAddress, 0);
    });
    Bind(ipAddress, 0);
}

void IPV6Socket::Bind(const IPV6Address & ipAddress, std::uint16_t port, 
                      std::uint32_t flowInfo, std::uint32_t scopeID)
{
    SCOPEDTRACE("", [&]{
        return 
            "ipAddress="+ serialization::Serialize(ipAddress, 0) +
            " port="+ serialization::Serialize(port, 0) +
            " flowInfo="+ serialization::Serialize(flowInfo, 0) +
            " scopeID="+ serialization::Serialize(scopeID, 0);
    });
    sockaddr_in6 bindAddress;
    memset(&bindAddress, 0, sizeof(bindAddress));
    bindAddress.sin6_family = AF_INET6;
    auto const & address = ipAddress.Address();
    std::copy(address.begin(), address.end(), std::begin(bindAddress.sin6_addr.s6_addr));
    bindAddress.sin6_port = utility::ToNetworkByteOrder(port);
    bindAddress.sin6_flowinfo = utility::ToNetworkByteOrder(flowInfo);
    bindAddress.sin6_scope_id = utility::ToNetworkByteOrder(scopeID);
    Socket::Bind((sockaddr *)&bindAddress, sizeof(bindAddress));
}

void IPV6Socket::Bind(std::uint16_t port)
{
    SCOPEDTRACE("", [&]{
        return 
            "port="+ serialization::Serialize(port, 0);
    });
    Bind(IPV6Address::Any, port);
}

void IPV6Socket::Bind(const IPV6EndPoint & ipEndPoint)
{
    SCOPEDTRACE("", [&]{
        return 
            "ipEndPoint="+ serialization::Serialize(ipEndPoint, 0);
    });
    Bind(ipEndPoint.IPAddress(), ipEndPoint.Port());
}

bool IPV6Socket::Connect(const IPV6EndPoint & serverAddress, SocketTimeout timeout)
{
    bool result {};
    SCOPEDTRACE("", [&]{
        return 
            "serverAddress="+ serialization::Serialize(serverAddress, 0) +
            " timeout="+ serialization::Serialize(timeout, 0) +
            " result="+ serialization::Serialize(result);
    });
    SockAddrIPV6 serverIPAddress = serverAddress.ConvertAddress();
    result = Socket::Connect(reinterpret_cast<const sockaddr *>(&serverIPAddress), sizeof(serverIPAddress), timeout);
    return result;
}

bool IPV6Socket::Accept(IPV6Socket & connectionSocket, IPV6EndPoint & clientAddress, SocketTimeout timeout)
{
    bool result {};
    SCOPEDTRACE("", [&]{
        return 
            "connectionSocket="+ serialization::Serialize(connectionSocket, 0) +
            "clientAddress="+ serialization::Serialize(clientAddress, 0) +
            " timeout="+ serialization::Serialize(timeout, 0) +
            " result="+ serialization::Serialize(result);
    });
    sockaddr_in6 clientIPAddress {};
    socklen_t clientIPAddressSize = sizeof(clientIPAddress);
    result = Socket::Accept(connectionSocket, reinterpret_cast<sockaddr *>(&clientIPAddress), &clientIPAddressSize, timeout);
    if (result)
    {
        ASSERT(clientIPAddressSize == sizeof(clientIPAddress));
        clientAddress = IPV6EndPoint(SockAddrIPV6(clientIPAddress));
    }
    return result;
}

void IPV6Socket::GetLocalAddress(IPV6EndPoint & ipEndPoint)
{
    SCOPEDTRACE("", [&]{
        return 
            "ipEndPoint="+ serialization::Serialize(ipEndPoint, 0);
    });
    sockaddr_in6 localAddress;
    socklen_t addressSize = sizeof(localAddress);
    Socket::GetLocalAddress((sockaddr *)&localAddress, &addressSize);
    ipEndPoint = IPV6EndPoint(IPV6Address(localAddress.sin6_addr), 
                                          utility::FromNetworkByteOrder(localAddress.sin6_port),
                                          utility::FromNetworkByteOrder(localAddress.sin6_flowinfo),
                                          utility::FromNetworkByteOrder(localAddress.sin6_scope_id));
}

void IPV6Socket::GetRemoteAddress(IPV6EndPoint & ipEndPoint)
{
    SCOPEDTRACE("", [&]{
        return 
            "ipEndPoint="+ serialization::Serialize(ipEndPoint, 0);
    });
    sockaddr_in6 localAddress;
    socklen_t addressSize = sizeof(localAddress);
    Socket::GetRemoteAddress((sockaddr *)&localAddress, &addressSize);
    ipEndPoint = IPV6EndPoint(IPV6Address(localAddress.sin6_addr),
                                          utility::FromNetworkByteOrder(localAddress.sin6_port),
                                          utility::FromNetworkByteOrder(localAddress.sin6_flowinfo),
                                          utility::FromNetworkByteOrder(localAddress.sin6_scope_id));
}

void IPV6Socket::SendTo(const IPV6EndPoint & ipEndPoint, const std::vector<uint8_t> & data, size_t bytesToSend)
{
    SCOPEDTRACE("", [&]{
        return 
            "ipEndPoint="+ serialization::Serialize(ipEndPoint, 0) + 
            " bytesToSend="+ serialization::Serialize(bytesToSend, 0);
    });
    SendTo(ipEndPoint.IPAddress(), ipEndPoint.Port(), ipEndPoint.FlowInfo(), ipEndPoint.ScopeID(),
           data, bytesToSend);
}

void IPV6Socket::SendTo(const IPV6Address & ipAddress, std::uint16_t port, 
                        std::uint32_t flowInfo, std::uint32_t scopeID,
                        const std::vector<uint8_t> & data, size_t bytesToSend)
{
    SCOPEDTRACE("", [&]{
        return 
            "ipAddress="+ serialization::Serialize(ipAddress, 0) + 
            " port="+ serialization::Serialize(port, 0) +
            " flowInfo="+ serialization::Serialize(flowInfo, 0) +
            " scopeID="+ serialization::Serialize(scopeID, 0) + 
            " bytesToSend="+ serialization::Serialize(bytesToSend, 0);
    });
    SendTo(ipAddress, port, flowInfo, scopeID, data.data(), bytesToSend);
}

void IPV6Socket::SendTo(const IPV6EndPoint & ipEndPoint,const uint8_t * data, size_t bytesToSend)
{
    SCOPEDTRACE("", [&]{
        return 
            "ipEndPoint="+ serialization::Serialize(ipEndPoint, 0) + 
            " bytesToSend="+ serialization::Serialize(bytesToSend, 0);
    });
    SendTo(ipEndPoint.IPAddress(), ipEndPoint.Port(), ipEndPoint.FlowInfo(), ipEndPoint.ScopeID(),
           data, bytesToSend);
}

void IPV6Socket::SendTo(const IPV6Address & ipAddress, std::uint16_t port, 
                        std::uint32_t flowInfo, std::uint32_t scopeID,
                        const uint8_t * data, size_t bytesToSend)
{
    SCOPEDTRACE("", [&]{
        return 
            "ipAddress="+ serialization::Serialize(ipAddress, 0) + 
            " port="+ serialization::Serialize(port, 0) +
            " flowInfo="+ serialization::Serialize(flowInfo, 0) +
            " scopeID="+ serialization::Serialize(scopeID, 0) + 
            " bytesToSend="+ serialization::Serialize(bytesToSend, 0);
    });
    sockaddr_in6 serverAddress;
    serverAddress.sin6_family = AF_INET6;
    auto const & address = ipAddress.Address();
    std::copy(address.begin(), address.end(), std::begin(serverAddress.sin6_addr.s6_addr));
    serverAddress.sin6_port = utility::ToNetworkByteOrder(port);
    serverAddress.sin6_flowinfo = utility::ToNetworkByteOrder(flowInfo);
    serverAddress.sin6_scope_id = utility::ToNetworkByteOrder(scopeID);

    Socket::SendTo(reinterpret_cast<sockaddr *>(&serverAddress), sizeof(serverAddress), data, bytesToSend);
}

std::vector<uint8_t> IPV6Socket::ReceiveFrom(IPV6EndPoint & ipEndPoint)
{
    size_t numBytes {};
    SCOPEDTRACE("", [&]{
        return 
            "ipEndPoint="+ serialization::Serialize(ipEndPoint, 0) + 
            " numBytes="+ serialization::Serialize(numBytes, 0);
    });
    uint8_t buffer[BufferSize];
    numBytes = ReceiveFrom(ipEndPoint, buffer, BufferSize);
    return std::vector<uint8_t>(&buffer[0], &buffer[numBytes]);
}

std::vector<uint8_t> IPV6Socket::ReceiveFrom(IPV6Address & ipAddress, std::uint16_t & port, 
                                             std::uint32_t & flowInfo, std::uint32_t & scopeID)
{
    size_t numBytes {};
    SCOPEDTRACE("", [&]{
        return 
            "ipAddress="+ serialization::Serialize(ipAddress, 0) + 
            " port="+ serialization::Serialize(port, 0) +
            " flowInfo="+ serialization::Serialize(flowInfo, 0) +
            " scopeID="+ serialization::Serialize(scopeID, 0) + 
            " numBytes="+ serialization::Serialize(numBytes, 0);
    });
    uint8_t buffer[BufferSize];
    numBytes = ReceiveFrom(ipAddress, port, flowInfo, scopeID, buffer, BufferSize);
    return std::vector<uint8_t>(&buffer[0], &buffer[numBytes]);
}

size_t IPV6Socket::ReceiveFrom(IPV6EndPoint & ipEndPoint, uint8_t * data, size_t bufferSize)
{
    size_t numBytes {};
    std::vector<uint8_t> result;
    SCOPEDTRACE("", [&]{
        return 
            "ipEndPoint="+ serialization::Serialize(ipEndPoint, 0) + 
            " numBytes="+ serialization::Serialize(numBytes, 0);
    });
    IPV6Address ipAddress;
    std::uint16_t port;
    std::uint32_t flowInfo;
    std::uint32_t scopeID;
    numBytes = ReceiveFrom(ipAddress, port, flowInfo, scopeID, data, bufferSize);
    ipEndPoint = IPV6EndPoint(ipAddress, port, flowInfo, scopeID);

    return numBytes;
}

size_t IPV6Socket::ReceiveFrom(IPV6Address & ipAddress, std::uint16_t & port, 
                               std::uint32_t & flowInfo, std::uint32_t & scopeID, 
                               uint8_t * data, size_t bufferSize)
{
    size_t numBytes {};
    SCOPEDTRACE("", [&]{
        return 
            "ipAddress="+ serialization::Serialize(ipAddress, 0) + 
            " port="+ serialization::Serialize(port, 0) +
            " flowInfo="+ serialization::Serialize(flowInfo, 0) +
            " scopeID="+ serialization::Serialize(scopeID, 0) + 
            " numBytes="+ serialization::Serialize(numBytes, 0);
    });
    sockaddr_in6 serverAddress;
    socklen_t length = sizeof(serverAddress);

    numBytes = Socket::ReceiveFrom((sockaddr *)&serverAddress, &length, data, bufferSize);
    if (numBytes > 0)
    {
        ipAddress = IPV6Address(serverAddress.sin6_addr);
        port = utility::FromNetworkByteOrder(serverAddress.sin6_port);
        flowInfo = utility::FromNetworkByteOrder(serverAddress.sin6_flowinfo);
        scopeID = utility::FromNetworkByteOrder(serverAddress.sin6_scope_id);
    }
    return numBytes;
}

std::ostream & operator <<(std::ostream & stream, const IPV6Socket & value)
{
    return stream << serialization::Serialize(value, 0);
}

} // namespace network
