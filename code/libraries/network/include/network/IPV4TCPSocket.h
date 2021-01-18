#pragma once

#include "network/IPV4Socket.h"

namespace network {

class IPV4TCPSocket
    : private IPV4Socket
{
public:
    IPV4TCPSocket(ISocketAPI & socketAPI);
    IPV4TCPSocket(const IPV4TCPSocket & other);
    IPV4TCPSocket(IPV4TCPSocket && other);

    IPV4TCPSocket & operator = (IPV4TCPSocket && other);

    using IPV4Socket::GetHandle;
    using IPV4Socket::SetHandle;

    using IPV4Socket::Family;
    using IPV4Socket::Type;

	void Open(SocketProtocol protocol = SocketProtocol::IP);
    using IPV4Socket::Close;
    using IPV4Socket::IsOpen;

    using IPV4Socket::SetSocketOptionWithLevel;
    using IPV4Socket::GetSocketOptionWithLevel;
    using IPV4Socket::SetSocketOption;
    using IPV4Socket::GetSocketOption;

    using IPV4Socket::GetSocketOptionBool;
    using IPV4Socket::SetSocketOptionBool;
    using IPV4Socket::GetSocketOptionInt;
    using IPV4Socket::SetSocketOptionInt;

    using IPV4Socket::GetBlockingMode;
    using IPV4Socket::SetBlockingMode;
    using IPV4Socket::GetReuseAddress;
    using IPV4Socket::SetReuseAddress;
    using IPV4Socket::GetReceiveTimeout;
    using IPV4Socket::SetReceiveTimeout;
    using IPV4Socket::GetSendTimeout;
    using IPV4Socket::SetSendTimeout;

    void Bind(const IPV4Address & ipAddress = IPV4Address::Any);
    void Bind(const IPV4Address & ipAddress, std::uint16_t port);
    void Bind(uint16_t port);
    void Bind(const IPV4EndPoint & ipEndPoint);

    using IPV4Socket::Connect;
    using IPV4Socket::Listen;
    bool Accept(IPV4TCPSocket & connectionSocket, IPV4EndPoint & clientAddress, SocketTimeout timeout);

    using IPV4Socket::GetLocalAddress;
    using IPV4Socket::GetRemoteAddress;

    using IPV4Socket::Receive;
    using IPV4Socket::Send;
};

} // namespace network
