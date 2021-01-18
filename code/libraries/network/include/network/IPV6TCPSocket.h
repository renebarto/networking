#pragma once

#include "network/IPV6Socket.h"

namespace network {

class IPV6TCPSocket
    : private IPV6Socket
{
public:
    IPV6TCPSocket(ISocketAPI & socketAPI);
    IPV6TCPSocket(const IPV6TCPSocket & other);
    IPV6TCPSocket(IPV6TCPSocket && other);

    IPV6TCPSocket & operator = (IPV6TCPSocket && other);

    using IPV6Socket::GetHandle;
    using IPV6Socket::SetHandle;

    using IPV6Socket::Family;
    using IPV6Socket::Type;

	void Open(SocketProtocol protocol = SocketProtocol::IP);
    using IPV6Socket::Close;
    using IPV6Socket::IsOpen;

    using IPV6Socket::SetSocketOptionWithLevel;
    using IPV6Socket::GetSocketOptionWithLevel;
    using IPV6Socket::SetSocketOption;
    using IPV6Socket::GetSocketOption;

    using IPV6Socket::GetSocketOptionBool;
    using IPV6Socket::SetSocketOptionBool;
    using IPV6Socket::GetSocketOptionInt;
    using IPV6Socket::SetSocketOptionInt;

    using IPV6Socket::GetBlockingMode;
    using IPV6Socket::SetBlockingMode;
    using IPV6Socket::GetReuseAddress;
    using IPV6Socket::SetReuseAddress;
    using IPV6Socket::GetReceiveTimeout;
    using IPV6Socket::SetReceiveTimeout;
    using IPV6Socket::GetSendTimeout;
    using IPV6Socket::SetSendTimeout;

    void Bind(const IPV6Address & ipAddress = IPV6Address::Any);
    void Bind(const IPV6Address & ipAddress, std::uint16_t port, std::uint32_t flowInfo = 0, std::uint32_t scopeID = 0);
    void Bind(std::uint16_t port);
    void Bind(const IPV6EndPoint & ipEndPoint);

    using IPV6Socket::Connect;
    using IPV6Socket::Listen;
    bool Accept(IPV6TCPSocket & connectionSocket, IPV6EndPoint & clientAddress, SocketTimeout timeout);

    using IPV6Socket::GetLocalAddress;
    using IPV6Socket::GetRemoteAddress;

    using IPV6Socket::Receive;
    using IPV6Socket::Send;
};

std::ostream & operator <<(std::ostream & stream, const IPV6TCPSocket & value);

} // namespace network
