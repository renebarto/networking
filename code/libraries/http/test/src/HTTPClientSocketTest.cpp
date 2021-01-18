#include "GoogleTest.h"

#include "network-osal/SocketAPI.h"
#include "http/HTTPClientSocket.h"

namespace http {

TEST(HTTPClientSocketTest, Construct)
{
    network::SocketAPI api;
    network::IPV4EndPoint address;
    HTTPClientSocket client(api, address);
    FAIL();
}

} // namespace http
