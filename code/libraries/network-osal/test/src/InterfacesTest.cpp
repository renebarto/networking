#include "GoogleTest.h"

#include "network-osal/Interfaces.h"
#include "network-osal/Interface.h"

#include "tracing/Tracing.h"
#include "utility/Serialization.h"

namespace network {

TEST(InterfacesTest, HasAtLeastLocalLoopInterface)
{
    tracing::Tracing::SetTracingFunctions(
        nullptr, 
        [](tracing::TraceCategory /*category*/) { return true; });

    Interfaces interfaces;
    std::string localLoopbackInterface;

    ASSERT_TRUE(interfaces.HaveLocalLoopbackInterface(localLoopbackInterface));
    const Interface & localLoopbackNIC = interfaces.GetInterface(localLoopbackInterface);
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
    Interfaces interfacesForLookup;
    std::string localLoopbackInterface;
    ASSERT_TRUE(interfacesForLookup.HaveLocalLoopbackInterface(localLoopbackInterface));

    Interfaces interfaces(localLoopbackInterface);
    ASSERT_TRUE(interfaces.HaveInterface(localLoopbackInterface));
    const Interface & localLoopbackNIC = interfaces.GetInterface(localLoopbackInterface);
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
    std::string localLoopbackInterface;
    ASSERT_TRUE(interfaces.HaveLocalLoopbackInterface(localLoopbackInterface));
    auto allInterfaces = interfaces.GetAllInterfaces();
    EXPECT_TRUE(allInterfaces.find(localLoopbackInterface) != allInterfaces.end());
}

TEST(InterfacesTest, GetLocalLoopbackInterfaces)
{
    Interfaces interfaces;
    std::string localLoopbackInterface;
    ASSERT_TRUE(interfaces.HaveLocalLoopbackInterface(localLoopbackInterface));
    auto localLoopbackInterfaces = interfaces.GetLocalLoopbackInterfaces();
    EXPECT_TRUE(localLoopbackInterfaces.find(localLoopbackInterface) != localLoopbackInterfaces.end());
}

TEST(InterfacesTest, GetIPV4Interfaces)
{
    Interfaces interfaces;
    std::string localLoopbackInterface;
    ASSERT_TRUE(interfaces.HaveLocalLoopbackInterface(localLoopbackInterface));
    auto ipV4Interfaces = interfaces.GetIPV4Interfaces();
    EXPECT_TRUE(ipV4Interfaces.find(localLoopbackInterface) != ipV4Interfaces.end());
}

TEST(InterfacesTest, GetIPV6Interfaces)
{
    Interfaces interfaces;
    std::string localLoopbackInterface;
    ASSERT_TRUE(interfaces.HaveLocalLoopbackInterface(localLoopbackInterface));
    auto ipV6Interfaces = interfaces.GetIPV6Interfaces();
    EXPECT_TRUE(ipV6Interfaces.find(localLoopbackInterface) != ipV6Interfaces.end());
}

} // namespace network
