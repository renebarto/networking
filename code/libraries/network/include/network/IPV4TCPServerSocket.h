#pragma once

#include "network/IPV4TCPSocket.h"

namespace network {

class IPV4TCPServerSocket
    : private IPV4TCPSocket
{
private:
    PortType m_port;
    int m_numListeners;
    std::chrono::milliseconds m_acceptTimeout;
    bool m_isInitialized;

public:
    IPV4TCPServerSocket(ISocketAPI & api, PortType port, int numListeners, std::chrono::milliseconds acceptTimeout);
    ~IPV4TCPServerSocket();

    bool Initialize();
    bool Uninitialize();
    bool IsInitialized() const;

    bool Accept(IPV4TCPSocket & clientSocket, IPV4EndPoint & clientAddress);

	using IPV4TCPSocket::Open;
    using IPV4TCPSocket::Close;
    using IPV4TCPSocket::GetLocalAddress;
    using IPV4TCPSocket::GetRemoteAddress;
    using IPV4TCPSocket::Receive;
    using IPV4TCPSocket::Send;
};

} // namespace network