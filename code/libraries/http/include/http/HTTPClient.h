#pragma once

#include "http/HTTPClientSocket.h"

namespace http {

class HTTPRequest;
class HTTPResponse;

class HTTPClient
{
public:
    HTTPClient(network::ISocketAPI & api, const network::IPV4EndPoint & address);
    HTTPClient(network::ISocketAPI & api, const std::string & serverName, network::PortType port);

    bool Connect();
    void Disconnect();

    bool Send(const HTTPRequest & request);
    bool Receive(HTTPResponse & response);
};

} // namespace http