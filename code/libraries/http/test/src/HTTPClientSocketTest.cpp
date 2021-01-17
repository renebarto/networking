#include "GoogleTest.h"

#include "http/HTTPClientSocket.h"

namespace http {

TEST(HTTPClientSocketTest, Construct)
{
    network::IPV4EndPoint address;
    HTTPClientSocket client(address);
    FAIL();
}

} // namespace http
