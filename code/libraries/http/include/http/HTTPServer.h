#pragma once

#include "core/WorkerThread.h"
#include "network/IPV4TCPSocket.h"
#include "http/SocketBlocking.h"

namespace http {

class HTTPServerThread 
    : public core::WorkerThread
{
private:
    network::PortType m_port;
    int m_numListeners;
    SocketBlocking m_blockingMode;

public:
    HTTPServerThread();
    
    bool Start(network::PortType port, int numListeners, SocketBlocking blocking);

    void Thread() override;    
};

class HTTPServer
{
private:
    HTTPServerThread m_serverThread;

public:
    HTTPServer();
    
    void Start(network::PortType port, int numListeners, SocketBlocking blocking);
    void Stop();

    bool IsStarted();
};

} // namespace http