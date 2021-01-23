#include "network/IPV4Socket.h"

#include "tracing/ScopedTracing.h"
#include "tracing/Tracing.h"
#include "utility/Assert.h"
#include "utility/Endian.h"
#include "utility/Error.h"

namespace serialization {

std::string Serialize(const network::IPV4Socket & value, int width)
{
    return Serialize(static_cast<int>(value.GetHandle(), width));
}

} // namespace serialization

namespace network {

static constexpr SocketFamily IPV4SocketFamily = SocketFamily::InternetV4;

IPV4Socket::IPV4Socket(ISocketAPI & socketAPI)
    : Socket(socketAPI)
{
}

IPV4Socket::IPV4Socket(ISocketAPI & socketAPI, SocketType socketType, SocketProtocol socketProtocol)
    : Socket(socketAPI, IPV4SocketFamily, socketType, socketProtocol)
{
}

IPV4Socket::IPV4Socket(IPV4Socket && other)
    : Socket(std::move(other))
{
}

IPV4Socket & IPV4Socket::operator = (IPV4Socket && other)
{
    if (this != & other)
    {
        Socket::operator = (std::move(other));
    }
    return *this;
}

void IPV4Socket::Bind(const IPV4Address & ipAddress)
{
    SCOPEDTRACE(nullptr, [&]{
        return utility::FormatString("ipAddress={}", 
            serialization::Serialize(ipAddress, 0));
    });
    Bind(ipAddress, 0);
}

void IPV4Socket::Bind(const IPV4Address & ipAddress, std::uint16_t port)
{
    SCOPEDTRACE(nullptr, [&]{
        return utility::FormatString("ipAddress={}  port={}",
            serialization::Serialize(ipAddress, 0),
            serialization::Serialize(port, 0));
    });
    sockaddr_in bindAddress;
    memset(&bindAddress, 0, sizeof(bindAddress));
    bindAddress.sin_family = AF_INET;
    bindAddress.sin_addr.s_addr = ipAddress.GetUInt32();
    bindAddress.sin_port = htons(port);
    Socket::Bind(reinterpret_cast<sockaddr *>(&bindAddress), sizeof(bindAddress));
}

void IPV4Socket::Bind(uint16_t port)
{
    SCOPEDTRACE(nullptr, [&]{
        return utility::FormatString("port={}",
            serialization::Serialize(port, 0));
    });
    Bind(IPV4Address::Any, port);
}

void IPV4Socket::Bind(const IPV4EndPoint & ipEndPoint)
{
    SCOPEDTRACE(nullptr, [&]{
        return utility::FormatString("ipEndPoint=[{}]",
            serialization::Serialize(ipEndPoint, 0));
    });
    Bind(ipEndPoint.IPAddress(), ipEndPoint.Port());
}

bool IPV4Socket::Connect(const IPV4EndPoint & serverAddress, SocketTimeout timeout)
{
    bool result {};
    SCOPEDTRACE(nullptr, [&]{
        return utility::FormatString("serverAddress={} timeout={} result={}",
            serialization::Serialize(serverAddress, 0),
            serialization::Serialize(timeout, 0),
            serialization::Serialize(result));
    });
    SockAddrIPV4 serverIPAddress = serverAddress.ConvertAddress();
    result = Socket::Connect(reinterpret_cast<const sockaddr *>(&serverIPAddress), sizeof(serverIPAddress), timeout);
    return result;
}

bool IPV4Socket::Accept(IPV4Socket & connectionSocket, IPV4EndPoint & clientAddress, SocketTimeout timeout)
{
    bool result {};
    SCOPEDTRACE(nullptr, [&]{
        return utility::FormatString("connectionSocket={} clientAddress=[{}] timeout={} result={}",
            serialization::Serialize(connectionSocket, 0),
            serialization::Serialize(clientAddress, 0),
            serialization::Serialize(timeout, 0),
            serialization::Serialize(result));
    });
    sockaddr_in clientIPAddress {};
    socklen_t clientIPAddressSize = sizeof(clientIPAddress);
    result = Socket::Accept(connectionSocket, reinterpret_cast<sockaddr *>(&clientIPAddress), &clientIPAddressSize, timeout);
    if (result)
    {
        ASSERT(clientIPAddressSize == sizeof(clientIPAddress));
        clientAddress = IPV4EndPoint(SockAddrIPV4(clientIPAddress));
    }
    return result;
}

void IPV4Socket::GetLocalAddress(IPV4EndPoint & ipEndPoint)
{
    SCOPEDTRACE(nullptr, [&]{
        return utility::FormatString("ipEndPoint=[{}]",
            serialization::Serialize(ipEndPoint, 0));
    });
    sockaddr_in localAddress;
    socklen_t addressSize = sizeof(localAddress);
    Socket::GetLocalAddress(reinterpret_cast<sockaddr *>(&localAddress), &addressSize);
    ipEndPoint = IPV4EndPoint(IPV4Address(localAddress.sin_addr), 
                                          utility::FromNetworkByteOrder(localAddress.sin_port));
}

void IPV4Socket::GetRemoteAddress(IPV4EndPoint & ipEndPoint)
{
    SCOPEDTRACE(nullptr, [&]{
        return utility::FormatString("ipEndPoint=[{}]",
            serialization::Serialize(ipEndPoint, 0));
    });
    sockaddr_in localAddress;
    socklen_t addressSize = sizeof(localAddress);
    Socket::GetRemoteAddress(reinterpret_cast<sockaddr *>(&localAddress), &addressSize);
    ipEndPoint = IPV4EndPoint(IPV4Address(localAddress.sin_addr),
                                          utility::FromNetworkByteOrder(localAddress.sin_port));
}

size_t IPV4Socket::ReceiveFrom(IPV4EndPoint & ipEndPoint, std::uint8_t * data, size_t bufferSize)
{
    size_t numBytes {};
    std::vector<uint8_t> result;
    SCOPEDTRACE([&]{ 
        return utility::FormatString("bufferSize={}",
            serialization::Serialize(bufferSize, 0));
    }, [&]{ 
        return utility::FormatString("result={} ipEndPoint=[{}] data=[{}]",
            serialization::Serialize(numBytes, 0),
            serialization::Serialize(ipEndPoint, 0),
            serialization::SerializeData(data, numBytes));
    });
    sockaddr_in serverAddress;
    socklen_t length = sizeof(serverAddress);

    numBytes = Socket::ReceiveFrom(reinterpret_cast<sockaddr *>(&serverAddress), &length, data, bufferSize);
    if (numBytes > 0)
    {
        IPV4Address ipAddress = IPV4Address(serverAddress.sin_addr);
        std::uint16_t port = utility::FromNetworkByteOrder(serverAddress.sin_port);
        ipEndPoint = IPV4EndPoint(ipAddress, port);
    }

    return numBytes;
}

bool IPV4Socket::SendTo(const IPV4EndPoint & ipEndPoint,const std::uint8_t * data, size_t bytesToSend)
{
    bool result {};
    SCOPEDTRACE(nullptr, [&]{
        return utility::FormatString("ipEndPoint=[{}] bytesToSend={}",
            serialization::Serialize(ipEndPoint, 0),
            serialization::Serialize(bytesToSend, 0));
    });
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = ipEndPoint.IPAddress().GetUInt32();
    serverAddress.sin_port = utility::ToNetworkByteOrder(ipEndPoint.Port());

    result = Socket::SendTo(reinterpret_cast<sockaddr *>(&serverAddress), sizeof(serverAddress), data, bytesToSend);
    return result;
}

size_t IPV4Socket::ReceiveBufferFrom(IPV4EndPoint & ipEndPoint, ByteBuffer & data, size_t bufferSize)
{
    size_t numBytes {};
    SCOPEDTRACE([&]{ 
        return utility::FormatString("bufferSize={}",
            serialization::Serialize(bufferSize, 0));
    }, [&]{ 
        return utility::FormatString("result={} ipEndPoint=[{}] data=[{}]",
            serialization::Serialize(numBytes, 0),
            serialization::Serialize(ipEndPoint, 0),
            serialization::SerializeData(data));
    });
    sockaddr_in serverAddress;
    socklen_t length = sizeof(serverAddress);

    numBytes = Socket::ReceiveBufferFrom(reinterpret_cast<sockaddr *>(&serverAddress), &length, data, bufferSize);
    if (numBytes > 0)
    {
        IPV4Address ipAddress = IPV4Address(serverAddress.sin_addr);
        std::uint16_t port = utility::FromNetworkByteOrder(serverAddress.sin_port);
        ipEndPoint = IPV4EndPoint(ipAddress, port);
    }

    return numBytes;
}

bool IPV4Socket::ReceiveBlockFrom(IPV4EndPoint & ipEndPoint, ByteBuffer & data, size_t bufferSize)
{
    bool result {};
    SCOPEDTRACE([&]{ 
        return utility::FormatString("bufferSize={}",
            serialization::Serialize(bufferSize, 0));
    }, [&]{ 
        return utility::FormatString("result={} ipEndPoint=[{}] data=[{}]",
            serialization::Serialize(result, 0),
            serialization::Serialize(ipEndPoint, 0),
            serialization::SerializeData(data));
    });
    sockaddr_in serverAddress;
    socklen_t length = sizeof(serverAddress);

    result = Socket::ReceiveBlockFrom(reinterpret_cast<sockaddr *>(&serverAddress), &length, data, bufferSize);
    if (result)
    {
        IPV4Address ipAddress = IPV4Address(serverAddress.sin_addr);
        std::uint16_t port = utility::FromNetworkByteOrder(serverAddress.sin_port);
        ipEndPoint = IPV4EndPoint(ipAddress, port);
    }
    return result;
}

bool IPV4Socket::SendBufferTo(const IPV4EndPoint & ipEndPoint, const ByteBuffer & data)
{
    bool result {};
    SCOPEDTRACE(nullptr, [&]{
        return utility::FormatString("ipEndPoint=[{}] bytesToSend={}",
            serialization::Serialize(ipEndPoint, 0),
            serialization::Serialize(data.size(), 0));
    });
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = ipEndPoint.IPAddress().GetUInt32();
    serverAddress.sin_port = utility::ToNetworkByteOrder(ipEndPoint.Port());

    result = Socket::SendBufferTo(reinterpret_cast<sockaddr *>(&serverAddress), sizeof(serverAddress), data);
    return result;
}

std::ostream & operator <<(std::ostream & stream, const IPV4Socket & value)
{
    return stream << serialization::Serialize(value, 0);
}

} // namespace network
