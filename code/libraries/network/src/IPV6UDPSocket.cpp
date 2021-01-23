#include "network/IPV6UDPSocket.h"

namespace network {

IPV6UDPSocket::IPV6UDPSocket(ISocketAPI & socketAPI)
    : IPV6Socket(socketAPI, SocketType::Datagram, SocketProtocol::IP)
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

} // namespace network