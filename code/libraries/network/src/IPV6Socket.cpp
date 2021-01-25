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

static constexpr SocketFamily IPV6SocketFamily = SocketFamily::InternetV6;

IPV6Socket::IPV6Socket(ISocketAPI & socketAPI)
    : Socket(socketAPI)
{
}

IPV6Socket::IPV6Socket(ISocketAPI & socketAPI, SocketType socketType, SocketProtocol socketProtocol)
    : Socket(socketAPI, IPV6SocketFamily, socketType, socketProtocol)
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

void IPV6Socket::Bind(const IPV6Address & ipAddress)
{
    SCOPEDTRACE(nullptr, [&]{
        return utility::FormatString("ipAddress={}", ipAddress);
    });
    Bind(ipAddress, 0);
}

void IPV6Socket::Bind(const IPV6Address & ipAddress, std::uint16_t port, 
                      std::uint32_t flowInfo, std::uint32_t scopeID)
{
    SCOPEDTRACE(nullptr, [&]{
        return utility::FormatString("ipAddress={} port={} flowInfo={} scopeID={}",
            ipAddress, port, flowInfo, scopeID);
    });
    sockaddr_in6 bindAddress;
    memset(&bindAddress, 0, sizeof(bindAddress));
    bindAddress.sin6_family = AF_INET6;
    auto const & address = ipAddress.Address();
    std::copy(address.begin(), address.end(), std::begin(bindAddress.sin6_addr.s6_addr));
    bindAddress.sin6_port = utility::ToNetworkByteOrder(port);
    bindAddress.sin6_flowinfo = utility::ToNetworkByteOrder(flowInfo);
    bindAddress.sin6_scope_id = utility::ToNetworkByteOrder(scopeID);
    Socket::Bind(reinterpret_cast<sockaddr *>(&bindAddress), sizeof(bindAddress));
}

void IPV6Socket::Bind(std::uint16_t port)
{
    SCOPEDTRACE(nullptr, [&]{
        return utility::FormatString("port={}", port);
    });
    Bind(IPV6Address::Any, port);
}

void IPV6Socket::Bind(const IPV6EndPoint & ipEndPoint)
{
    SCOPEDTRACE(nullptr, [&]{
        return utility::FormatString("ipEndPoint=[{}]", ipEndPoint);
    });
    Bind(ipEndPoint.IPAddress(), ipEndPoint.Port());
}

bool IPV6Socket::Connect(const IPV6EndPoint & serverAddress, SocketTimeout timeout)
{
    bool result {};
    SCOPEDTRACE(nullptr, [&]{
        return utility::FormatString("serverAddress=[{}] timeout={} result={}",
            serverAddress, timeout, result);
    });
    SockAddrIPV6 serverIPAddress = serverAddress.ConvertAddress();
    result = Socket::Connect(reinterpret_cast<const sockaddr *>(&serverIPAddress), sizeof(serverIPAddress), timeout);
    return result;
}

bool IPV6Socket::Accept(IPV6Socket & connectionSocket, IPV6EndPoint & clientAddress, SocketTimeout timeout)
{
    bool result {};
    SCOPEDTRACE(nullptr, [&]{
        return utility::FormatString("connectionSocket={} clientAddress=[{}] timeout={} result={}",
            serialization::Serialize(connectionSocket, 0), clientAddress, timeout, result);
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
    SCOPEDTRACE(nullptr, [&]{
        return utility::FormatString("ipEndPoint={}", ipEndPoint);
    });
    sockaddr_in6 localAddress;
    socklen_t addressSize = sizeof(localAddress);
    Socket::GetLocalAddress(reinterpret_cast<sockaddr *>(&localAddress), &addressSize);
    ipEndPoint = IPV6EndPoint(IPV6Address(localAddress.sin6_addr), 
                                          utility::FromNetworkByteOrder(localAddress.sin6_port),
                                          utility::FromNetworkByteOrder(localAddress.sin6_flowinfo),
                                          utility::FromNetworkByteOrder(localAddress.sin6_scope_id));
}

void IPV6Socket::GetRemoteAddress(IPV6EndPoint & ipEndPoint)
{
    SCOPEDTRACE(nullptr, [&]{
        return utility::FormatString("ipEndPoint={}", ipEndPoint);
    });
    sockaddr_in6 localAddress;
    socklen_t addressSize = sizeof(localAddress);
    Socket::GetRemoteAddress(reinterpret_cast<sockaddr *>(&localAddress), &addressSize);
    ipEndPoint = IPV6EndPoint(IPV6Address(localAddress.sin6_addr),
                                          utility::FromNetworkByteOrder(localAddress.sin6_port),
                                          utility::FromNetworkByteOrder(localAddress.sin6_flowinfo),
                                          utility::FromNetworkByteOrder(localAddress.sin6_scope_id));
}

size_t IPV6Socket::ReceiveFrom(IPV6EndPoint & ipEndPoint, uint8_t * data, size_t bufferSize)
{
    size_t numBytes {};
    std::vector<uint8_t> result;
    SCOPEDTRACE([&]{ 
        return utility::FormatString("bufferSize={}", bufferSize);
    }, [&]{ 
        return utility::FormatString("result={} ipEndPoint=[{}] data=[{}]",
            numBytes,
            ipEndPoint,
            serialization::SerializeData(data, numBytes));
    });
    sockaddr_in6 serverAddress;
    socklen_t length = sizeof(serverAddress);

    numBytes = Socket::ReceiveFrom(reinterpret_cast<sockaddr *>(&serverAddress), &length, data, bufferSize);
    if (numBytes > 0)
    {
        IPV6Address ipAddress = IPV6Address(serverAddress.sin6_addr);
        std::uint16_t port = utility::FromNetworkByteOrder(serverAddress.sin6_port);
        std::uint32_t flowInfo = utility::FromNetworkByteOrder(serverAddress.sin6_flowinfo);
        std::uint32_t scopeID = utility::FromNetworkByteOrder(serverAddress.sin6_scope_id);
        ipEndPoint = IPV6EndPoint(ipAddress, port, flowInfo, scopeID);
    }
    return numBytes;
}

std::size_t IPV6Socket::SendTo(const IPV6EndPoint & ipEndPoint,const uint8_t * data, size_t bytesToSend)
{
    std::size_t result {};
    SCOPEDTRACE([&]{
        return utility::FormatString("ipEndPoint=[{}] bytesToSend={}", ipEndPoint, bytesToSend);
    }, [&]{
        return utility::FormatString("result={}", result);
    });
    sockaddr_in6 serverAddress;
    serverAddress.sin6_family = AF_INET6;
    auto const & address = ipEndPoint.IPAddress().Address();
    std::copy(address.begin(), address.end(), std::begin(serverAddress.sin6_addr.s6_addr));
    serverAddress.sin6_port = utility::ToNetworkByteOrder(ipEndPoint.Port());
    serverAddress.sin6_flowinfo = utility::ToNetworkByteOrder(ipEndPoint.FlowInfo());
    serverAddress.sin6_scope_id = utility::ToNetworkByteOrder(ipEndPoint.ScopeID());

    result = Socket::SendTo(reinterpret_cast<sockaddr *>(&serverAddress), sizeof(serverAddress), data, bytesToSend);
    return result;
}

size_t IPV6Socket::ReceiveBufferFrom(IPV6EndPoint & ipEndPoint, ByteBuffer & data, size_t bufferSize)
{
    size_t numBytes {};
    SCOPEDTRACE([&]{ 
        return utility::FormatString("bufferSize={}", bufferSize);
    }, [&]{ 
        return utility::FormatString("result={} ipEndPoint=[{}] data=[{}]",
            numBytes,
            ipEndPoint,
            serialization::SerializeData(data));
    });
    sockaddr_in6 serverAddress;
    socklen_t length = sizeof(serverAddress);

    numBytes = Socket::ReceiveBufferFrom(reinterpret_cast<sockaddr *>(&serverAddress), &length, data, bufferSize);
    if (numBytes > 0)
    {
        IPV6Address ipAddress = IPV6Address(serverAddress.sin6_addr);
        std::uint16_t port = utility::FromNetworkByteOrder(serverAddress.sin6_port);
        std::uint32_t flowInfo = utility::FromNetworkByteOrder(serverAddress.sin6_flowinfo);
        std::uint32_t scopeID = utility::FromNetworkByteOrder(serverAddress.sin6_scope_id);
        ipEndPoint = IPV6EndPoint(ipAddress, port, flowInfo, scopeID);
    }
    return numBytes;
}

bool IPV6Socket::ReceiveBlockFrom(IPV6EndPoint & ipEndPoint, ByteBuffer & data, size_t bufferSize)
{
    bool result {};
    SCOPEDTRACE([&]{ 
        return utility::FormatString("bufferSize={}", bufferSize);
    }, [&]{ 
        return utility::FormatString("result={} ipEndPoint=[{}] data=[{}]",
            result,
            ipEndPoint,
            serialization::SerializeData(data));
    });
    sockaddr_in6 serverAddress;
    socklen_t length = sizeof(serverAddress);

    result = Socket::ReceiveBlockFrom(reinterpret_cast<sockaddr *>(&serverAddress), &length, data, bufferSize);
    if (result)
    {
        IPV6Address ipAddress = IPV6Address(serverAddress.sin6_addr);
        std::uint16_t port = utility::FromNetworkByteOrder(serverAddress.sin6_port);
        std::uint32_t flowInfo = utility::FromNetworkByteOrder(serverAddress.sin6_flowinfo);
        std::uint32_t scopeID = utility::FromNetworkByteOrder(serverAddress.sin6_scope_id);
        ipEndPoint = IPV6EndPoint(ipAddress, port, flowInfo, scopeID);
    }
    return result;
}

bool IPV6Socket::SendBufferTo(const IPV6EndPoint & ipEndPoint, const std::vector<uint8_t> & data)
{
    bool result {};
    SCOPEDTRACE([&]{
        return utility::FormatString("ipEndPoint=[{}] data=[{}]",
            ipEndPoint,
            serialization::SerializeData(data));
    }, [&]{
        return utility::FormatString("result={}", result);
    });
    sockaddr_in6 serverAddress;
    serverAddress.sin6_family = AF_INET6;
    auto const & address = ipEndPoint.IPAddress().Address();
    std::copy(address.begin(), address.end(), std::begin(serverAddress.sin6_addr.s6_addr));
    serverAddress.sin6_port = utility::ToNetworkByteOrder(ipEndPoint.Port());
    serverAddress.sin6_flowinfo = utility::ToNetworkByteOrder(ipEndPoint.FlowInfo());
    serverAddress.sin6_scope_id = utility::ToNetworkByteOrder(ipEndPoint.ScopeID());

    result = Socket::SendBufferTo(reinterpret_cast<sockaddr *>(&serverAddress), sizeof(serverAddress), data);
    return result;
}

std::ostream & operator <<(std::ostream & stream, const IPV6Socket & value)
{
    return stream << serialization::Serialize(value, 0);
}

} // namespace network
