#include "network/IPV4UDPSocket.h"

namespace network {

IPV4UDPSocket::IPV4UDPSocket()
    : IPV4Socket(SocketType::Datagram)
{    
}

IPV4UDPSocket::IPV4UDPSocket(const IPV4UDPSocket & other)
    : IPV4Socket(other)
{    
}

IPV4UDPSocket::IPV4UDPSocket(IPV4UDPSocket && other)
    : IPV4Socket(std::move(other))
{    
}

IPV4UDPSocket & IPV4UDPSocket::operator = (IPV4UDPSocket && other)
{
    if (this != &other)
    {
        IPV4Socket::operator = (std::move(other));
    }
    return *this;
}

void IPV4UDPSocket::Open(SocketProtocol protocol)
{
    IPV4Socket::Open(SocketType::Datagram, protocol);
}

void IPV4UDPSocket::Bind(const IPV4Address & ipAddress)
{
    IPV4Socket::Bind(ipAddress);    
}

void IPV4UDPSocket::Bind(const IPV4Address & ipAddress, std::uint16_t port)
{
    IPV4Socket::Bind(ipAddress, port);
}

void IPV4UDPSocket::Bind(uint16_t port)
{
    IPV4Socket::Bind(port);
}

void IPV4UDPSocket::Bind(const IPV4EndPoint & ipEndPoint)
{
    IPV4Socket::Bind(ipEndPoint);
}

void IPV4UDPSocket::SendTo(const IPV4EndPoint & ipEndPoint, const std::vector<uint8_t> & data, size_t bytesToSend)
{
    IPV4Socket::SendTo(ipEndPoint, data, bytesToSend);    
}

void IPV4UDPSocket::SendTo(const IPV4Address & ipAddress, std::uint16_t port, const std::vector<uint8_t> & data, size_t bytesToSend)
{
    IPV4Socket::SendTo(ipAddress, port, data, bytesToSend);    
}

void IPV4UDPSocket::SendTo(const IPV4EndPoint & ipEndPoint, const uint8_t * data, size_t bytesToSend)
{
    IPV4Socket::SendTo(ipEndPoint, data, bytesToSend);    
}

void IPV4UDPSocket::SendTo(const IPV4Address & ipAddress, std::uint16_t port, const uint8_t * data, size_t bytesToSend)
{
    IPV4Socket::SendTo(ipAddress, port, data, bytesToSend);    
}

std::vector<uint8_t> IPV4UDPSocket::ReceiveFrom(IPV4EndPoint & ipEndPoint)
{
    return IPV4Socket::ReceiveFrom(ipEndPoint);    
}

std::vector<uint8_t> IPV4UDPSocket::ReceiveFrom(IPV4Address & ipAddress, std::uint16_t & port)
{
    return IPV4Socket::ReceiveFrom(ipAddress, port);    
}

size_t IPV4UDPSocket::ReceiveFrom(IPV4EndPoint & ipEndPoint, uint8_t * data, size_t bufferSize)
{
    return IPV4Socket::ReceiveFrom(ipEndPoint, data, bufferSize);
}

size_t IPV4UDPSocket::ReceiveFrom(IPV4Address & ipAddress, std::uint16_t & port, uint8_t * data, size_t bufferSize)
{
    return IPV4Socket::ReceiveFrom(ipAddress, port, data, bufferSize);
}

} // namespace network