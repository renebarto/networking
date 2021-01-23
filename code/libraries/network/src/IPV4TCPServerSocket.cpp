#include "network/IPV4TCPServerSocket.h"

#include "tracing/Logging.h"
#include "tracing/ScopedTracing.h"

namespace network {

IPV4TCPServerSocket::IPV4TCPServerSocket(ISocketAPI & api, PortType port, int numListeners, SocketBlocking blockingMode)
    : IPV4TCPSocket(api)
    , m_port(port)
    , m_numListeners(numListeners)
    , m_blockingMode(blockingMode)
    , m_isInitialized()
{
    SCOPEDTRACE(nullptr, nullptr);
}

IPV4TCPServerSocket::~IPV4TCPServerSocket()
{
    SCOPEDTRACE(nullptr, nullptr);
}

bool IPV4TCPServerSocket::Initialize()
{
    bool result {};
    SCOPEDTRACE([this] () { return utility::FormatString("initialized={}", m_isInitialized); }, 
                [&] () { return utility::FormatString("result={}", result); });
    if (m_isInitialized)
    {
        LogWarning(__FILE__, __LINE__, __func__, "Already initialized");
        result = true;
    }
    else
    {
        try {
            Open();
            if (m_blockingMode == SocketBlocking::Off)
                SetBlockingMode(false);
            Bind(m_port);
            Listen(m_numListeners);
            m_isInitialized = true;
            result = true;
        }
        catch (std::exception & e)
        {
            LogError(__FILE__, __LINE__, __func__, "Exception thrown: {}", e.what());
        }
    }
    return result;
}

bool IPV4TCPServerSocket::Uninitialize()
{
    bool result {};
    SCOPEDTRACE([this] () { return utility::FormatString("initialized={}", m_isInitialized); }, 
                [&] () { return utility::FormatString("result={}", result); });
    if (m_isInitialized)
    {
        m_isInitialized = false;
        result = true;
    }
    return result;
}

bool IPV4TCPServerSocket::IsInitialized() const
{
    SCOPEDTRACE(nullptr, 
                [this] () { return utility::FormatString("result={}", m_isInitialized); });
    return m_isInitialized;
}

bool IPV4TCPServerSocket::Accept(IPV4TCPSocket & clientSocket, IPV4EndPoint & clientAddress, SocketTimeout timeout)
{
    bool result {};
    SCOPEDTRACE([&] () { return utility::FormatString("timeout={}", timeout); }, 
                [&] () { return utility::FormatString("clientAddress=[{}] result={}", clientAddress, result); });
    if (!IsInitialized())
        result = false;
    else
    {
        result = IPV4TCPSocket::Accept(clientSocket, clientAddress, timeout);
    }
    return result;
}

} // namespace network