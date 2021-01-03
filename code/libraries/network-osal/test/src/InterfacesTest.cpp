#include "GoogleTest.h"

#include "network-osal/Interfaces.h"
#include "network-osal/Interface.h"

#include "tracing/Tracing.h"
#include "utility/Serialization.h"

namespace network {

static const std::string LocalLoopbackInterface = "lo";

bool TraceEnabled(tracing::TraceCategory /*category*/)
{
    return true;
}

TEST(InterfacesTest, HasAtLeastLocalLoopInterface)
{
    Interfaces interfaces;
    EXPECT_TRUE(interfaces.HaveInterface(LocalLoopbackInterface));
    const Interface & localLoopbackNIC = interfaces.GetInterface(LocalLoopbackInterface);
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

TEST(InterfacesTest, ConstructOnlyLocalLoopInterface)
{
    Interfaces interfaces(LocalLoopbackInterface);
    EXPECT_TRUE(interfaces.HaveInterface(LocalLoopbackInterface));
    const Interface & localLoopbackNIC = interfaces.GetInterface(LocalLoopbackInterface);
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

TEST(InterfacesTest, GetAllInterfaces)
{
    Interfaces interfaces;
    EXPECT_TRUE(interfaces.HaveInterface(LocalLoopbackInterface));
    auto allInterfaces = interfaces.GetAllInterfaces();
    EXPECT_TRUE(allInterfaces.find(LocalLoopbackInterface) != allInterfaces.end());
}

TEST(InterfacesTest, GetLocalLoopbackInterfaces)
{
    Interfaces interfaces;
    EXPECT_TRUE(interfaces.HaveInterface(LocalLoopbackInterface));
    auto localLoopbackInterfaces = interfaces.GetLocalLoopbackInterfaces();
    EXPECT_TRUE(localLoopbackInterfaces.find(LocalLoopbackInterface) != localLoopbackInterfaces.end());
}

TEST(InterfacesTest, GetIPV4Interfaces)
{
    Interfaces interfaces;
    EXPECT_TRUE(interfaces.HaveInterface(LocalLoopbackInterface));
    auto ipV4Interfaces = interfaces.GetIPV4Interfaces();
    EXPECT_TRUE(ipV4Interfaces.find(LocalLoopbackInterface) != ipV4Interfaces.end());
}

TEST(InterfacesTest, GetIPV6Interfaces)
{
    Interfaces interfaces;
    EXPECT_TRUE(interfaces.HaveInterface(LocalLoopbackInterface));
    auto ipV6Interfaces = interfaces.GetIPV6Interfaces();
    EXPECT_TRUE(ipV6Interfaces.find(LocalLoopbackInterface) != ipV6Interfaces.end());
}

} // namespace network
