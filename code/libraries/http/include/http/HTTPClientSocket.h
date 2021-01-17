#pragma once

#include "network/IPV4TCPSocket.h"

namespace http {

class HTTPClientSocket : private network::IPV4TCPSocket
{
private:
    network::IPV4EndPoint m_serverEndPoint;

public:
    HTTPClientSocket(const network::IPV4EndPoint & address);
    ~HTTPClientSocket();

    bool Connect();
    void Disconnect();

    using network::IPV4TCPSocket::Receive;
    using network::IPV4TCPSocket::Send;
};

} // namespace http