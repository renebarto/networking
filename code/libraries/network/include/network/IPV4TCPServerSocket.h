#pragma once

#include "network/IPV4TCPSocket.h"
#include "network/SocketBlocking.h"

namespace network {

class IPV4TCPServerSocket
    : private IPV4TCPSocket
{
private:
    PortType m_port;
    int m_numListeners;
    SocketBlocking m_blockingMode;
    bool m_isInitialized;

public:
    IPV4TCPServerSocket(ISocketAPI & api, PortType port, int numListeners, SocketBlocking blockingMode);
    ~IPV4TCPServerSocket();

    bool Initialize();
    bool Uninitialize();
    bool IsInitialized() const;

    bool Accept(IPV4TCPSocket & clientSocket, IPV4EndPoint & clientAddress, SocketTimeout timeout);

	using IPV4TCPSocket::Open;
    using IPV4TCPSocket::Close;
    using IPV4TCPSocket::GetLocalAddress;
    using IPV4TCPSocket::GetRemoteAddress;
    using IPV4TCPSocket::Receive;
    using IPV4TCPSocket::Send;
};

} // namespace network