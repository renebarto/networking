#include "GoogleTest.h"

#include "http/HTTPServerSocket.h"

namespace http {

TEST(HTTPServerSocketTest, Construct)
{
    HTTPServerSocket server(8080, 1, SocketBlocking::On);
    FAIL();
}

} // namespace http
