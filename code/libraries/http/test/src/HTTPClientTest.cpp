#include "GoogleTest.h"

#include "http/HTTPClient.h"

namespace http {

TEST(HTTPClientTest, ConstructDefault)
{
    HTTPClient client("localhost", 8080);
    FAIL();
}

} // namespace http
