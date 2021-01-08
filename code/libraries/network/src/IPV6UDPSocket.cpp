#include "network/IPV6UDPSocket.h"

namespace network {

IPV6UDPSocket::IPV6UDPSocket()
    : IPV6Socket(SocketType::Datagram)
{    
}

IPV6UDPSocket::IPV6UDPSocket(const IPV6UDPSocket & other)
    : IPV6Socket(other)
{    
}

IPV6UDPSocket::IPV6UDPSocket(IPV6UDPSocket && other)
    : IPV6Socket(std::move(other))
{    
}

IPV6UDPSocket & IPV6UDPSocket::operator = (IPV6UDPSocket && other)
{
    if (this != &other)
    {
        IPV6Socket::operator = (std::move(other));
    }
    return *this;
}

void IPV6UDPSocket::Open(SocketProtocol protocol)
{
    IPV6Socket::Open(SocketType::Datagram, protocol);
}

void IPV6UDPSocket::Bind(const IPV6Address & ipAddress)
{
    IPV6Socket::Bind(ipAddress);    
}

void IPV6UDPSocket::Bind(const IPV6Address & ipAddress, std::uint16_t port, std::uint32_t flowInfo, std::uint32_t scopeID)
{
    IPV6Socket::Bind(ipAddress, port, flowInfo, scopeID);
}

void IPV6UDPSocket::Bind(uint16_t port)
{
    IPV6Socket::Bind(port);
}

void IPV6UDPSocket::Bind(const IPV6EndPoint & ipEndPoint)
{
    IPV6Socket::Bind(ipEndPoint);
}

void IPV6UDPSocket::SendTo(const IPV6EndPoint & ipEndPoint, const std::vector<uint8_t> & data, size_t bytesToSend)
{
    IPV6Socket::SendTo(ipEndPoint, data, bytesToSend);    
}

void IPV6UDPSocket::SendTo(const IPV6Address & ipAddress, std::uint16_t port, std::uint32_t flowInfo, std::uint32_t scopeID, const std::vector<uint8_t> & data, size_t bytesToSend)
{
    IPV6Socket::SendTo(ipAddress, port, flowInfo, scopeID, data, bytesToSend);    
}

void IPV6UDPSocket::SendTo(const IPV6EndPoint & ipEndPoint, const uint8_t * data, size_t bytesToSend)
{
    IPV6Socket::SendTo(ipEndPoint, data, bytesToSend);    
}

void IPV6UDPSocket::SendTo(const IPV6Address & ipAddress, std::uint16_t port, std::uint32_t flowInfo, std::uint32_t scopeID, const uint8_t * data, size_t bytesToSend)
{
    IPV6Socket::SendTo(ipAddress, port, flowInfo, scopeID, data, bytesToSend);    
}

std::vector<uint8_t> IPV6UDPSocket::ReceiveFrom(IPV6EndPoint & ipEndPoint)
{
    return IPV6Socket::ReceiveFrom(ipEndPoint);    
}

std::vector<uint8_t> IPV6UDPSocket::ReceiveFrom(IPV6Address & ipAddress, std::uint16_t & port, std::uint32_t & flowInfo, std::uint32_t & scopeID)
{
    return IPV6Socket::ReceiveFrom(ipAddress, port, flowInfo, scopeID);    
}

size_t IPV6UDPSocket::ReceiveFrom(IPV6EndPoint & ipEndPoint, uint8_t * data, size_t bufferSize)
{
    return IPV6Socket::ReceiveFrom(ipEndPoint, data, bufferSize);
}

size_t IPV6UDPSocket::ReceiveFrom(IPV6Address & ipAddress, std::uint16_t & port, std::uint32_t & flowInfo, std::uint32_t & scopeID, uint8_t * data, size_t bufferSize)
{
    return IPV6Socket::ReceiveFrom(ipAddress, port, flowInfo, scopeID, data, bufferSize);
}

} // namespace network