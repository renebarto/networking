#include "network/IPV6TCPSocket.h"

namespace network {

IPV6TCPSocket::IPV6TCPSocket(ISocketAPI & socketAPI)
    : IPV6Socket(socketAPI, SocketType::Stream)
{    
}

IPV6TCPSocket::IPV6TCPSocket(const IPV6TCPSocket & other)
    : IPV6Socket(other)
{
}

IPV6TCPSocket::IPV6TCPSocket(IPV6TCPSocket && other)
    : IPV6Socket(std::move(other))
{
}


IPV6TCPSocket & IPV6TCPSocket::operator = (IPV6TCPSocket && other)
{
    if (this != &other)
    {
        IPV6Socket::operator = (std::move(other));
    }
    return *this;
}

void IPV6TCPSocket::Open(SocketProtocol protocol)
{
    IPV6Socket::Open(SocketType::Stream, protocol);
}

void IPV6TCPSocket::Bind(const IPV6Address & ipAddress)
{
    IPV6Socket::Bind(ipAddress);
}

void IPV6TCPSocket::Bind(const IPV6Address & ipAddress, std::uint16_t port, std::uint32_t flowInfo, std::uint32_t scopeID)
{
    IPV6Socket::Bind(ipAddress, port, flowInfo, scopeID);
}

void IPV6TCPSocket::Bind(uint16_t port)
{
    IPV6Socket::Bind(port);
}

void IPV6TCPSocket::Bind(const IPV6EndPoint & ipEndPoint)
{
    IPV6Socket::Bind(ipEndPoint);
}

bool IPV6TCPSocket::Accept(IPV6TCPSocket & connectionSocket, IPV6EndPoint & clientAddress, SocketTimeout timeout)
{
    return IPV6Socket::Accept(connectionSocket, clientAddress, timeout);
}

} // namespace network