#include "http/HTTPClientSocket.h"

namespace http {

HTTPClientSocket::HTTPClientSocket(network::ISocketAPI & api, const network::IPV4EndPoint & /*address*/)
    : IPV4TCPSocket(api)
    , m_serverEndPoint()
{
}

HTTPClientSocket::~HTTPClientSocket()
{

}

bool HTTPClientSocket::Connect()
{
    return false;
}

void HTTPClientSocket::Disconnect()
{

}

} // namespace http