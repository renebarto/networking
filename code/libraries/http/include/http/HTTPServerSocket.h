#pragma once

#include "network/IPV4TCPSocket.h"
#include "http/SocketBlocking.h"

namespace http {

class HTTPServerSocket : private network::IPV4TCPSocket
{
private:
    network::PortType m_port;
    int m_numListeners;
    SocketBlocking m_blockingMode;
    bool m_isInitialized;

public:
    HTTPServerSocket(network::ISocketAPI & api, network::PortType port, int numListeners, SocketBlocking blockingMode);
    ~HTTPServerSocket();

    bool Initialize();
    bool Uninitialize();
    bool IsInitialized();

    bool Accept(network::IPV4TCPSocket & clientSocket);

    using network::IPV4TCPSocket::Receive;
    using network::IPV4TCPSocket::Send;
};

} // namespace http