#include "GoogleTest.h"

#include "network-osal/SocketAPI.h"
#include "tracing/ScopedTracing.h"
#include "tracing/Tracing.h"
#include "http/HTTPServer.h"

namespace http {

TEST(HTTPServerTest, Construct)
{
    network::SocketAPI api;
    HTTPServer server(api);
    EXPECT_FALSE(server.IsStarted());
}

TEST(HTTPServerTest, ConstructDefault)
{
    network::SocketAPI api;
    HTTPServer server(api);
    server.Start(8080, 1, SocketBlocking::On);
    EXPECT_TRUE(server.IsStarted());
}

} // namespace http
