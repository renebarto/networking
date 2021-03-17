#include "network/IPV4TCPServerConnectionThread.h"

#include "tracing/ScopedTracing.h"

namespace network {

IPV4TCPServerConnectionThread::IPV4TCPServerConnectionThread(ISocketAPI & api)
    : core::threading::ActiveObject("IPV4TCPServerConnectionThread")
    , m_socketAPI(api)
    , m_clientSocket(api)
    , m_clientEndPoint()
{
    SCOPEDTRACE(nullptr, nullptr);
}

IPV4TCPServerConnectionThread::~IPV4TCPServerConnectionThread()
{
    if (IsStarted())
        Stop();
}

bool IPV4TCPServerConnectionThread::Start(IPV4TCPSocket && clientSocket, const IPV4EndPoint & clientEndPoint)
{
    m_clientSocket = std::move(clientSocket);
    m_clientEndPoint = clientEndPoint;
    SCOPEDTRACE([this] () { return utility::FormatString("Thread starting for client {}", m_clientEndPoint); }, nullptr);
    Create();

    return true;
}

void IPV4TCPServerConnectionThread::Stop()
{
    SCOPEDTRACE([this] () { return utility::FormatString("Thread stopping for client {}", m_clientEndPoint); }, nullptr);
    m_clientSocket.Close();
    Kill();
}

bool IPV4TCPServerConnectionThread::IsStarted()
{
    return IsRunning();
}

} // namespace network
