#include "GoogleTest.h"

#include "http/HTTPServer.h"

namespace http {

TEST(HTTPServerTest, Construct)
{
    HTTPServer server;
    EXPECT_FALSE(server.IsStarted());
}

TEST(HTTPServerTest, ConstructDefault)
{
    HTTPServer server;
    server.Start(8080, 1, SocketBlocking::On);
    EXPECT_TRUE(server.IsStarted());
}

} // namespace http
