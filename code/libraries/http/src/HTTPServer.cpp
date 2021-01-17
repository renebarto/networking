#include "http/HTTPServer.h"

#include "http/HTTPServerSocket.h"

namespace http {

HTTPServerThread::HTTPServerThread()
    : core::WorkerThread("HTTPServer")
    , m_port()
    , m_numListeners()
    , m_blockingMode()
{
}

bool HTTPServerThread::Start(network::PortType port, int numListeners, SocketBlocking blockingMode)
{
    m_port = port;
    m_numListeners = numListeners;
    m_blockingMode = blockingMode;
    Create();

    return true;
}

void HTTPServerThread::Thread()
{
    HTTPServerSocket serverSocket(m_port, m_numListeners, m_blockingMode);
}

HTTPServer::HTTPServer()

{

}

void HTTPServer::Start(network::PortType /*port*/, int /*numListeners*/, SocketBlocking /*blocking*/)
{

}

void HTTPServer::Stop()
{

}

bool HTTPServer::IsStarted()
{
    return m_serverThread.IsRunning();
}

} // namespace http