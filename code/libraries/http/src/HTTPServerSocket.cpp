#include "http/HTTPServerSocket.h"

#include "tracing/Logging.h"

namespace http {

HTTPServerSocket::HTTPServerSocket(network::ISocketAPI & api, network::PortType port, int numListeners, SocketBlocking blockingMode)
    : IPV4TCPSocket(api)
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
    if (m_isInitialized)
    {
        return true;
    }
    try {
        if (m_blockingMode == SocketBlocking::Off)
            SetBlockingMode(false);
        Bind(m_port);
        Listen(m_numListeners);
    }
    catch (std::exception & e)
    {
        LogError(__FILE__, __LINE__, __func__, "Exception thrown: {}", e.what());
        return false;
    }
    m_isInitialized = true;
    return true;
}

bool HTTPServerSocket::Uninitialize()
{
    return false;
}

bool HTTPServerSocket::IsInitialized()
{
    return m_isInitialized;
}

bool HTTPServerSocket::Accept(network::IPV4TCPSocket & /*clientSocket*/)
{
    return false;
}


} // namespace http