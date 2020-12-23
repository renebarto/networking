#include "GoogleTest.h"

#include "network/Interfaces.h"
#include "network/Interface.h"

#include "tracing/Tracing.h"
#include "utility/Serialization.h"

namespace network {

bool TraceEnabled(tracing::TraceCategory /*category*/)
{
    return true;
}


TEST(InterfacesTest, HasAtLeastLocalLoopInterface)
{
    Interfaces interfaces;
    EXPECT_TRUE(interfaces.HaveInterface("lo"));
    const Interface & localLoopbackNIC = interfaces.GetInterface("lo");
    for (auto const & addressInfo : localLoopbackNIC.Addresses())
    {
        if (addressInfo.address.IsIPV4Address())
        {
            EXPECT_EQ("127.0.0.1", serialization::Serialize(addressInfo.address.GetIPV4Address(), 0));
        }
        if (addressInfo.address.IsIPV6Address())
        {
            EXPECT_EQ("::1", serialization::Serialize(addressInfo.address.GetIPV6Address(), 0));
        }
    }
}

} // namespace network
