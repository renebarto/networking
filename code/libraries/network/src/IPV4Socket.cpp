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

static const size_t BufferSize = 4096;
static constexpr SocketFamily IPV4SocketFamily = SocketFamily::InternetV4;

IPV4Socket::IPV4Socket(ISocketAPI & socketAPI)
    : Socket(socketAPI)
{
}

IPV4Socket::IPV4Socket(ISocketAPI & socketAPI, SocketType socketType)
    : Socket(socketAPI, IPV4SocketFamily, socketType)
{
}

IPV4Socket::IPV4Socket(const IPV4Socket & other)
    : Socket(other)
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
        *(Socket *)this = std::move(other);
    }
    return *this;
}

void IPV4Socket::Open(SocketType socketType, SocketProtocol protocol)
{
    SCOPEDTRACE(nullptr, [&]{
        return 
            "socketType="+ serialization::Serialize(socketType, 0) +
            " protocol="+ serialization::Serialize(protocol, 0);
    });
    Socket::Open(IPV4SocketFamily, socketType, protocol);
}
    
void IPV4Socket::Bind(const IPV4Address & ipAddress)
{
    SCOPEDTRACE(nullptr, [&]{
        return 
            "ipAddress="+ serialization::Serialize(ipAddress, 0);
    });
    Bind(ipAddress, 0);
}

void IPV4Socket::Bind(const IPV4Address & ipAddress, std::uint16_t port)
{
    SCOPEDTRACE(nullptr, [&]{
        return 
            "ipAddress="+ serialization::Serialize(ipAddress, 0) +
            " port="+ serialization::Serialize(port, 0);
    });
    sockaddr_in bindAddress;
    memset(&bindAddress, 0, sizeof(bindAddress));
    bindAddress.sin_family = AF_INET;
    bindAddress.sin_addr.s_addr = ipAddress.GetUInt32();
    bindAddress.sin_port = htons(port);
    Socket::Bind((sockaddr *)&bindAddress, sizeof(bindAddress));
}

void IPV4Socket::Bind(uint16_t port)
{
    SCOPEDTRACE(nullptr, [&]{
        return 
            "port="+ serialization::Serialize(port, 0);
    });
    Bind(IPV4Address::Any, port);
}

void IPV4Socket::Bind(const IPV4EndPoint & ipEndPoint)
{
    SCOPEDTRACE(nullptr, [&]{
        return 
            "ipEndPoint="+ serialization::Serialize(ipEndPoint, 0);
    });
    Bind(ipEndPoint.IPAddress(), ipEndPoint.Port());
}

bool IPV4Socket::Connect(const IPV4EndPoint & serverAddress, SocketTimeout timeout)
{
    bool result {};
    SCOPEDTRACE(nullptr, [&]{
        return 
            "serverAddress="+ serialization::Serialize(serverAddress, 0) +
            " timeout="+ serialization::Serialize(timeout, 0) +
            " result="+ serialization::Serialize(result);
    });
    SockAddrIPV4 serverIPAddress = serverAddress.ConvertAddress();
    result = Socket::Connect(reinterpret_cast<const sockaddr *>(&serverIPAddress), sizeof(serverIPAddress), timeout);
    return result;
}

bool IPV4Socket::Accept(IPV4Socket & connectionSocket, IPV4EndPoint & clientAddress, SocketTimeout timeout)
{
    bool result {};
    SCOPEDTRACE(nullptr, [&]{
        return 
            "connectionSocket="+ serialization::Serialize(connectionSocket, 0) +
            " clientAddress="+ serialization::Serialize(clientAddress, 0) +
            " timeout="+ serialization::Serialize(timeout, 0) +
            " result="+ serialization::Serialize(result);
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
        return 
            "ipEndPoint="+ serialization::Serialize(ipEndPoint, 0);
    });
    sockaddr_in localAddress;
    socklen_t addressSize = sizeof(localAddress);
    Socket::GetLocalAddress((sockaddr *)&localAddress, &addressSize);
    ipEndPoint = IPV4EndPoint(IPV4Address(localAddress.sin_addr), 
                                          utility::FromNetworkByteOrder(localAddress.sin_port));
}

void IPV4Socket::GetRemoteAddress(IPV4EndPoint & ipEndPoint)
{
    SCOPEDTRACE(nullptr, [&]{
        return 
            "ipEndPoint="+ serialization::Serialize(ipEndPoint, 0);
    });
    sockaddr_in localAddress;
    socklen_t addressSize = sizeof(localAddress);
    Socket::GetRemoteAddress((sockaddr *)&localAddress, &addressSize);
    ipEndPoint = IPV4EndPoint(IPV4Address(localAddress.sin_addr),
                                          utility::FromNetworkByteOrder(localAddress.sin_port));
}

void IPV4Socket::SendTo(const IPV4EndPoint & ipEndPoint, const std::vector<uint8_t> & data, size_t bytesToSend)
{
    SCOPEDTRACE(nullptr, [&]{
        return 
            "ipEndPoint="+ serialization::Serialize(ipEndPoint, 0) + 
            " bytesToSend="+ serialization::Serialize(bytesToSend, 0);
    });
    SendTo(ipEndPoint.IPAddress(), ipEndPoint.Port(), data, bytesToSend);
}

void IPV4Socket::SendTo(const IPV4Address & ipAddress, std::uint16_t port, const std::vector<uint8_t> & data, size_t bytesToSend)
{
    SCOPEDTRACE(nullptr, [&]{
        return 
            "ipAddress="+ serialization::Serialize(ipAddress, 0) + 
            " port="+ serialization::Serialize(port, 0) + 
            " bytesToSend="+ serialization::Serialize(bytesToSend, 0);
    });
    SendTo(ipAddress, port, data.data(), bytesToSend);
}

void IPV4Socket::SendTo(const IPV4EndPoint & ipEndPoint,const uint8_t * data, size_t bytesToSend)
{
    SCOPEDTRACE(nullptr, [&]{
        return 
            "ipEndPoint="+ serialization::Serialize(ipEndPoint, 0) + 
            " bytesToSend="+ serialization::Serialize(bytesToSend, 0);
    });
    SendTo(ipEndPoint.IPAddress(), ipEndPoint.Port(), data, bytesToSend);
}

void IPV4Socket::SendTo(const IPV4Address & ipAddress, std::uint16_t port, const uint8_t * data, size_t bytesToSend)
{
    SCOPEDTRACE(nullptr, [&]{
        return 
            "ipAddress="+ serialization::Serialize(ipAddress, 0) + 
            " port="+ serialization::Serialize(port, 0) + 
            " bytesToSend="+ serialization::Serialize(bytesToSend, 0);
    });
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = ipAddress.GetUInt32();
    serverAddress.sin_port = utility::ToNetworkByteOrder(port);

    Socket::SendTo(reinterpret_cast<sockaddr *>(&serverAddress), sizeof(serverAddress), data, bytesToSend);
}

std::vector<uint8_t> IPV4Socket::ReceiveFrom(IPV4EndPoint & ipEndPoint)
{
    size_t numBytes {};
    SCOPEDTRACE(nullptr, [&]{
        return 
            "ipEndPoint="+ serialization::Serialize(ipEndPoint, 0) + 
            " numBytes="+ serialization::Serialize(numBytes, 0);
    });
    uint8_t buffer[BufferSize];
    numBytes = ReceiveFrom(ipEndPoint, buffer, BufferSize);
    return std::vector<uint8_t>(&buffer[0], &buffer[numBytes]);
}

std::vector<uint8_t> IPV4Socket::ReceiveFrom(IPV4Address & ipAddress, std::uint16_t & port)
{
    size_t numBytes {};
    SCOPEDTRACE(nullptr, [&]{
        return 
            "ipAddress="+ serialization::Serialize(ipAddress, 0) + 
            " port="+ serialization::Serialize(port, 0) + 
            " numBytes="+ serialization::Serialize(numBytes, 0);
    });
    uint8_t buffer[BufferSize];
    numBytes = ReceiveFrom(ipAddress, port, buffer, BufferSize);
    return std::vector<uint8_t>(&buffer[0], &buffer[numBytes]);
}

size_t IPV4Socket::ReceiveFrom(IPV4EndPoint & ipEndPoint, uint8_t * data, size_t bufferSize)
{
    size_t numBytes {};
    std::vector<uint8_t> result;
    SCOPEDTRACE(nullptr, [&]{
        return 
            "ipEndPoint="+ serialization::Serialize(ipEndPoint, 0) + 
            " numBytes="+ serialization::Serialize(numBytes, 0);
    });
    IPV4Address ipAddress;
    std::uint16_t port;
    numBytes = ReceiveFrom(ipAddress, port, data, bufferSize);
    ipEndPoint = IPV4EndPoint(ipAddress, port);

    return numBytes;
}

size_t IPV4Socket::ReceiveFrom(IPV4Address & ipAddress, std::uint16_t & port, uint8_t * data, size_t bufferSize)
{
    size_t numBytes {};
    SCOPEDTRACE(nullptr, [&]{
        return 
            "ipAddress="+ serialization::Serialize(ipAddress, 0) + 
            " port="+ serialization::Serialize(port, 0) + 
            " numBytes="+ serialization::Serialize(numBytes, 0);
    });
    sockaddr_in serverAddress;
    socklen_t length = sizeof(serverAddress);

    numBytes = Socket::ReceiveFrom((sockaddr *)&serverAddress, &length, data, bufferSize);
    if (numBytes > 0)
    {
        ipAddress = IPV4Address(serverAddress.sin_addr);
        port = utility::FromNetworkByteOrder(serverAddress.sin_port);
    }
    return numBytes;
}

std::ostream & operator <<(std::ostream & stream, const IPV4Socket & value)
{
    return stream << serialization::Serialize(value, 0);
}

} // namespace network
