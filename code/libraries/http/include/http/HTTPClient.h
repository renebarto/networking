#pragma once

#include "http/HTTPClientSocket.h"

namespace http {

class HTTPRequest;
class HTTPResponse;

class HTTPClient
{
public:
    HTTPClient(const network::IPV4EndPoint & address);
    HTTPClient(const std::string & serverName, network::PortType port);

    bool Connect();
    void Disconnect();

    bool Send(const HTTPRequest & request);
    bool Receive(HTTPResponse & response);
};

} // namespace http