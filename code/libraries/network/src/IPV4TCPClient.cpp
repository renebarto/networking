#include "network/IPV4TCPClient.h"

#include "tracing/ScopedTracing.h"

namespace network {

IPV4TCPClient::IPV4TCPClient(ISocketAPI & api, const IPV4EndPoint & serverEndPoint)
    : m_api(api)
    , m_clientSocket(api)
    , m_serverEndPoint(serverEndPoint)
{
    SCOPEDTRACE(nullptr, 
                [this] () { return utility::FormatString("endPoint=[{}]", m_serverEndPoint); });
}

IPV4TCPClient::IPV4TCPClient(ISocketAPI & api, const IPV4Address & serverAddress, PortType serverPort)
    : m_api(api)
    , m_clientSocket(api)
    , m_serverEndPoint(serverAddress, serverPort)
{
    SCOPEDTRACE(nullptr, 
                [this] () { return utility::FormatString("endPoint=[{}]", m_serverEndPoint); });
}

bool IPV4TCPClient::Connect(std::chrono::milliseconds timeout)
{
    bool result {};
    SCOPEDTRACE([&] () { return utility::FormatString("timeout={}", timeout); }, 
                [&] () { return utility::FormatString("result={}", result); });
    m_clientSocket.Open();
    result = m_clientSocket.Connect(m_serverEndPoint, timeout);
    return result;
}

void IPV4TCPClient::Disconnect()
{
    SCOPEDTRACE(nullptr, nullptr);
    m_clientSocket.Disconnect();
}

bool IPV4TCPClient::IsConnected() const
{
    bool result {};
    SCOPEDTRACE(nullptr, 
                [&] () { return utility::FormatString("result={}", result); });
    result = m_clientSocket.IsConnected();
    return result;
}

void IPV4TCPClient::GetLocalAddress(IPV4EndPoint & ipEndPoint)
{
    m_clientSocket.GetLocalAddress(ipEndPoint);
}

void IPV4TCPClient::GetRemoteAddress(IPV4EndPoint & ipEndPoint)
{
    m_clientSocket.GetRemoteAddress(ipEndPoint);
}

std::size_t IPV4TCPClient::Receive(std::uint8_t * data, std::size_t bufferSize, int flags)
{
    return m_clientSocket.Receive(data, bufferSize, flags);
}

std::size_t IPV4TCPClient::Send(const std::uint8_t * data, std::size_t bytesToSend, int flags)
{
    return m_clientSocket.Send(data, bytesToSend, flags);
}

bool IPV4TCPClient::ReceiveBlock(ByteBuffer & data, std::size_t bufferSize, int flags)
{
    return m_clientSocket.ReceiveBlock(data, bufferSize, flags);
}

std::size_t IPV4TCPClient::ReceiveBuffer(ByteBuffer & data, std::size_t bufferSize, int flags)
{
    return m_clientSocket.ReceiveBuffer(data, bufferSize, flags);
}

bool IPV4TCPClient::SendBuffer(const ByteBuffer & data, int flags)
{
    return m_clientSocket.SendBuffer(data, flags);
}

} // namespace network