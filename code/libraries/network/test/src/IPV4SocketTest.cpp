#include "GoogleTest.h"

#include "network/IPV4Socket.h"

namespace network {

TEST(IPV4SocketTest, Construct)
{
    IPV4Socket target;
    EXPECT_EQ(InvalidHandleValue, target.GetHandle());
    EXPECT_FALSE(target.IsOpen());
}

} // namespace network