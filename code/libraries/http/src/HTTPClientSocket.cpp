#include "http/HTTPClientSocket.h"

namespace http {

HTTPClientSocket::HTTPClientSocket(const network::IPV4EndPoint & /*address*/)
    : IPV4TCPSocket()
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