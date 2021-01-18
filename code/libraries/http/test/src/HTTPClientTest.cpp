#include "GoogleTest.h"

#include "network-osal/SocketAPI.h"
#include "http/HTTPClient.h"

namespace http {

TEST(HTTPClientTest, ConstructDefault)
{
    network::SocketAPI api;
    HTTPClient client(api, "localhost", 8080);
    FAIL();
}

} // namespace http
