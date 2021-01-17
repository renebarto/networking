#include "http/HTTPServerSocket.h"

#include "tracing/Logging.h"

namespace http {

HTTPServerSocket::HTTPServerSocket(network::PortType port, int numListeners, SocketBlocking blockingMode)
    : IPV4TCPSocket()
    , m_port(port)
    , m_numListeners(numListeners)
    , m_blockingMode(blockingMode)
{
}

HTTPServerSocket::~HTTPServerSocket()
{

}

bool HTTPServerSocket::Initialize()
{
    try {
        if (m_blockingMode == SocketBlocking::Off)
            SetBlockingMode(false);
        Bind(m_port);
        Listen(m_numListeners);
    }
    catch (std::exception & e)
    {
        LogError(__FILE__, __LINE__, __func__, "Exception thrown: {}", e.what());
    }
    return true;
}

bool HTTPServerSocket::Uninitialize()
{
    return false;
}

bool HTTPServerSocket::Accept(network::IPV4TCPSocket & /*clientSocket*/)
{
    return false;
}


} // namespace http