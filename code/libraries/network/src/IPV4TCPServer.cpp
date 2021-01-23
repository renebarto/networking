#include "network/IPV4TCPServer.h"

#include "tracing/ScopedTracing.h"
#include "network/IPV4TCPServerSocket.h"

namespace network {

IPV4TCPServer::IPV4TCPServer(IPV4TCPServerThread & serverThread)
    : m_serverThread(serverThread)
{
    SCOPEDTRACE(nullptr, 
                nullptr);
}

IPV4TCPServer::~IPV4TCPServer()
{
    SCOPEDTRACE(nullptr, 
                nullptr);
    if (IsStarted())
        Stop();
}

void IPV4TCPServer::Start(PortType port, int numListeners, SocketBlocking blocking)
{
    SCOPEDTRACE([&] () { return utility::FormatString("port={}, numListeners={}, blocking={}", port, numListeners, blocking == SocketBlocking::On ? "On" : "Off"); }, 
                nullptr);
    m_serverThread.Start(port, numListeners, blocking);
}

void IPV4TCPServer::Stop()
{
    SCOPEDTRACE(nullptr, 
                nullptr);
    m_serverThread.Stop();
}

bool IPV4TCPServer::IsStarted()
{
    SCOPEDTRACE(nullptr, 
                nullptr);
    return m_serverThread.IsStarted();
}

} // namespace network
