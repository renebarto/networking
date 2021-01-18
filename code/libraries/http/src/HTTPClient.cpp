#include "http/HTTPClient.h"

#include "http/HTTPRequest.h"
#include "http/HTTPResponse.h"

namespace http {

HTTPClient::HTTPClient(network::ISocketAPI & /*api*/, const network::IPV4EndPoint & /*address*/)
{

}

HTTPClient::HTTPClient(network::ISocketAPI & /*api*/, const std::string & /*serverName*/, network::PortType /*port*/)
{
}

bool HTTPClient::Connect()
{
    return false;
}

void HTTPClient::Disconnect()
{

}

bool HTTPClient::Send(const HTTPRequest & /*request*/)
{
    return false;
}

bool HTTPClient::Receive(HTTPResponse & /*response*/)
{
    return false;
}

} // namespace http