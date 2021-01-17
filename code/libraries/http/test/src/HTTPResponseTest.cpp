#include "GoogleTest.h"

#include "http/HTTPResponse.h"

namespace http {

TEST(HTTPResponseTest, ConstructDefault)
{
    HTTPResponse request;
    FAIL();
}

} // namespace http
