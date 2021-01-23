#pragma once

#include "network/IPV4TCPClientSocket.h"

namespace network {

class HTTPRequest;
class HTTPResponse;

class IPV4TCPClient
{
private:
    ISocketAPI & m_api;
    IPV4TCPClientSocket m_clientSocket;
    IPV4EndPoint m_serverEndPoint;

public:
    IPV4TCPClient(ISocketAPI & api, const IPV4EndPoint & serverEndPoint);
    IPV4TCPClient(ISocketAPI & api, const IPV4Address & serverAddress, PortType serverPort);

    const IPV4EndPoint & ServerEndPoint() const { return m_serverEndPoint; }
    bool Connect(SocketTimeout timeout);
    void Disconnect();
    bool IsConnected() const;

    void GetLocalAddress(IPV4EndPoint & ipEndPoint);
    void GetRemoteAddress(IPV4EndPoint & ipEndPoint);

    std::size_t Receive(std::uint8_t * data, std::size_t bufferSize, int flags);
    bool        Send(const std::uint8_t * data, std::size_t bytesToSend, int flags);
    bool        ReceiveBlock(ByteBuffer & data, std::size_t bufferSize, int flags);
    std::size_t ReceiveBuffer(ByteBuffer & data, std::size_t bufferSize, int flags);
    bool        SendBuffer(const ByteBuffer & data, int flags);
};

} // namespace network