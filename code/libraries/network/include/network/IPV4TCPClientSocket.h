#pragma once

#include "network/IPV4TCPSocket.h"

namespace network {

class IPV4TCPClientSocket
    : private IPV4TCPSocket
{
private:
    IPV4EndPoint m_serverEndPoint;
    bool m_isConnected;

public:
    IPV4TCPClientSocket(ISocketAPI & api);
    ~IPV4TCPClientSocket();

    bool Connect(const IPV4EndPoint & address, SocketTimeout timeout);
    void Disconnect();
    bool IsConnected() const;

	using IPV4TCPSocket::Open;
    using IPV4TCPSocket::Close;
    using IPV4TCPSocket::GetLocalAddress;
    using IPV4TCPSocket::GetRemoteAddress;
    using IPV4TCPSocket::Receive;
    using IPV4TCPSocket::Send;
};

} // namespace network