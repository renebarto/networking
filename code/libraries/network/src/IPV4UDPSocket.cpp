#include "network/IPV4UDPSocket.h"

namespace network {

IPV4UDPSocket::IPV4UDPSocket(ISocketAPI & socketAPI)
    : IPV4Socket(socketAPI, SocketType::Datagram, SocketProtocol::IP)
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

} // namespace network