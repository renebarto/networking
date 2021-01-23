#include "network/IPV4TCPSocket.h"

namespace network {

IPV4TCPSocket::IPV4TCPSocket(ISocketAPI & socketAPI)
    : IPV4Socket(socketAPI, SocketType::Stream, SocketProtocol::IP)
{    
}

IPV4TCPSocket::IPV4TCPSocket(IPV4TCPSocket && other)
    : IPV4Socket(std::move(other))
{
}

IPV4TCPSocket & IPV4TCPSocket::operator = (IPV4TCPSocket && other)
{
    if (this != &other)
    {
        IPV4Socket::operator = (std::move(other));
    }
    return *this;
}

void IPV4TCPSocket::Bind(const IPV4Address & ipAddress)
{
    IPV4Socket::Bind(ipAddress);
}

void IPV4TCPSocket::Bind(const IPV4Address & ipAddress, std::uint16_t port)
{
    IPV4Socket::Bind(ipAddress, port);
}

void IPV4TCPSocket::Bind(uint16_t port)
{
    IPV4Socket::Bind(port);
}

void IPV4TCPSocket::Bind(const IPV4EndPoint & ipEndPoint)
{
    IPV4Socket::Bind(ipEndPoint);
}

bool IPV4TCPSocket::Accept(IPV4TCPSocket & connectionSocket, IPV4EndPoint & clientAddress, SocketTimeout timeout)
{
    return IPV4Socket::Accept(connectionSocket, clientAddress, timeout);
}

} // namespace network