#pragma once

#include <vector>
#include "network/IPV4TCPSocket.h"
#include "network/SocketBlocking.h"
#include "network/IPV4TCPServerThread.h"

namespace network {

class IPV4TCPServer
{
private:
    IPV4TCPServerThread & m_serverThread;

public:
    IPV4TCPServer(IPV4TCPServerThread & serverThread);
    ~IPV4TCPServer();
    
    void Start(PortType port, int numListeners, SocketBlocking blocking);
    void Stop();
    bool IsStarted();
};

} // namespace network