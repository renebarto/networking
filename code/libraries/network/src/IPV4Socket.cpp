#include "network/IPV4Socket.h"

#include "tracing/ScopedTracing.h"
#include "tracing/Tracing.h"
#include "utility/Endian.h"
#include "utility/Error.h"

namespace network {

static const size_t BufferSize = 4096;

IPV4Socket::IPV4Socket()
    : Socket()
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

void IPV4Socket::Bind(const IPV4Address & ipAddress)
{
    Bind(ipAddress, 0);
}

void IPV4Socket::Bind(const IPV4Address & ipAddress, uint16_t port)
{
    sockaddr_in bindAddress;
    memset(&bindAddress, 0, sizeof(bindAddress));
    bindAddress.sin_family = AF_INET;
    bindAddress.sin_addr.s_addr = ipAddress.GetUInt32();
    bindAddress.sin_port = htons(port);
    Socket::Bind((sockaddr *)&bindAddress, sizeof(bindAddress));
}

void IPV4Socket::Bind(uint16_t port)
{
    Bind(IPV4Address::Any, port);
}

void IPV4Socket::Bind(const IPV4EndPoint & ipEndPoint)
{
    Bind(ipEndPoint.IPAddress(), ipEndPoint.Port());
}

void IPV4Socket::GetLocalAddress(IPV4EndPoint & ipEndPoint)
{
    sockaddr_in localAddress;
    socklen_t addressSize = sizeof(localAddress);
    Socket::GetLocalAddress((sockaddr *)&localAddress, &addressSize);
    ipEndPoint = IPV4EndPoint(IPV4Address(localAddress.sin_addr), 
                                          utility::FromNetworkByteOrder(localAddress.sin_port));
}

void IPV4Socket::GetRemoteAddress(IPV4EndPoint & ipEndPoint)
{
    sockaddr_in localAddress;
    socklen_t addressSize = sizeof(localAddress);
    Socket::GetRemoteAddress((sockaddr *)&localAddress, &addressSize);
    ipEndPoint = IPV4EndPoint(IPV4Address(localAddress.sin_addr),
                                          utility::FromNetworkByteOrder(localAddress.sin_port));
}

void IPV4Socket::SendTo(const IPV4EndPoint & ipEndPoint, const std::vector<uint8_t> & data, size_t bytesToSend)
{
    SendTo(ipEndPoint.IPAddress(), ipEndPoint.Port(), data, bytesToSend);
}

void IPV4Socket::SendTo(const IPV4Address & ipAddress, uint16_t port, const std::vector<uint8_t> & data, size_t bytesToSend)
{
    // if (bytesToSend > data.Size())
    //     throw core::ArgumentOutOfRangeException(__func__, __FILE__, __LINE__, "bytesToSend", "Must be within size of data");

    SendTo(ipAddress, port, data.data(), bytesToSend);
}

void IPV4Socket::SendTo(const IPV4EndPoint & ipEndPoint,const uint8_t * data, size_t bytesToSend)
{
    SendTo(ipEndPoint.IPAddress(), ipEndPoint.Port(), data, bytesToSend);
}

void IPV4Socket::SendTo(const IPV4Address & ipAddress, uint16_t port, const uint8_t * data, size_t bytesToSend)
{
    sockaddr_in serverAddress;
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = utility::ToNetworkByteOrder(ipAddress.GetUInt32());
    serverAddress.sin_port = utility::ToNetworkByteOrder(port);

    Socket::SendTo(reinterpret_cast<sockaddr *>(&serverAddress), sizeof(serverAddress), data, bytesToSend);
}

std::vector<uint8_t> IPV4Socket::ReceiveFrom(IPV4EndPoint & ipEndPoint)
{
    uint8_t buffer[BufferSize];
    size_t numBytes = ReceiveFrom(ipEndPoint, buffer, BufferSize);
    return std::vector<uint8_t>(&buffer[0], &buffer[numBytes]);
}

std::vector<uint8_t> IPV4Socket::ReceiveFrom(IPV4Address & ipAddress, uint16_t & port)
{
    uint8_t buffer[BufferSize];
    size_t numBytes = ReceiveFrom(ipAddress, port, buffer, BufferSize);
    return std::vector<uint8_t>(&buffer[0], &buffer[numBytes]);
}

size_t IPV4Socket::ReceiveFrom(IPV4EndPoint & ipEndPoint, uint8_t * data, size_t bufferSize)
{
    IPV4Address ipAddress;
    uint16_t port;
    size_t numBytes = ReceiveFrom(ipAddress, port, data, bufferSize);
    ipEndPoint = IPV4EndPoint(ipAddress, port);

    return numBytes;
}

size_t IPV4Socket::ReceiveFrom(IPV4Address & ipAddress, uint16_t & port, uint8_t * data, size_t bufferSize)
{
    sockaddr_in serverAddress;
    socklen_t length = sizeof(serverAddress);

    size_t numBytes = Socket::ReceiveFrom((sockaddr *)&serverAddress, &length, data, bufferSize);
    if (numBytes > 0)
    {
        ipAddress = IPV4Address(serverAddress.sin_addr);
        port = utility::FromNetworkByteOrder(serverAddress.sin_port);
    }
    return numBytes;
}

} // namespace network

namespace serialization {

void Serialize(const network::IPV4Socket & value, int width)
{
    Serialize(static_cast<int>(value.GetHandle(), width));
}

} // namespace serialization