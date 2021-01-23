#include "network/IPV4TCPClientSocket.h"

#include "tracing/Logging.h"
#include "tracing/ScopedTracing.h"

namespace network {

IPV4TCPClientSocket::IPV4TCPClientSocket(ISocketAPI & api)
    : IPV4TCPSocket(api)
    , m_serverEndPoint()
    , m_isConnected()
{
    SCOPEDTRACE(nullptr, nullptr);
}

IPV4TCPClientSocket::~IPV4TCPClientSocket()
{
    SCOPEDTRACE(nullptr, nullptr);
}

bool IPV4TCPClientSocket::Connect(const IPV4EndPoint & address, SocketTimeout timeout)
{
    bool result {};
    bool isConnected = m_isConnected; // needed due to problems in SCOPEDTRACE macro
    SCOPEDTRACE([&] () { return utility::FormatString("isConnected={} address=[{}] timeout={}", isConnected, address, timeout); }, 
                [&] () { return utility::FormatString("result={}", result); });
    
    if (m_isConnected)
    {
        LogWarning(__FILE__, __LINE__, __func__, "Already connected");
        result = false;
    }
    else
    {
        // Open socket in case it was closed
        if (!IsOpen())
            Open();
        bool connected = IPV4TCPSocket::Connect(address, timeout);
        if (connected)
        {
            m_serverEndPoint = address;
            m_isConnected = true;
            result = true;
        }
    }
    return result;
}

void IPV4TCPClientSocket::Disconnect()
{
    SCOPEDTRACE([this] () { return utility::FormatString("isConnected={}", m_isConnected); }, 
                nullptr);
    Close();
    m_isConnected = false;
}

bool IPV4TCPClientSocket::IsConnected() const
{
    SCOPEDTRACE(nullptr, 
                [this] () { return utility::FormatString("result={}", m_isConnected); });
    return m_isConnected;
}

} // namespace network