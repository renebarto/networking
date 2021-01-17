#include "GoogleTest.h"

#include "http/HTTPRequest.h"

namespace http {

TEST(HTTPRequestTest, ConstructDefault)
{
    HTTPRequest request;
    FAIL();
}

} // namespace http
