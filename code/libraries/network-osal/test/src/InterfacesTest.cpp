#include "GoogleTest.h"

#include "network-osal/Interfaces.h"
#include "network-osal/Interface.h"

#include "tracing/Tracing.h"
#include "utility/Serialization.h"

namespace network {

class InterfacesTest : public ::testing::Test
{
public:
    tracing::CategorySet<tracing::TraceCategory> m_savedTraceFilter;

    InterfacesTest()
        : m_savedTraceFilter()
    {}

    void SetUp() override
    {
        m_savedTraceFilter = tracing::GetDefaultTraceFilter();
    }
    void TearDown() override
    {
        tracing::SetDefaultTraceFilter(m_savedTraceFilter);
    }
};

TEST_F(InterfacesTest, HasAtLeastLocalLoopInterface)
{
    tracing::SetDefaultTraceFilter(tracing::TraceCategory::All);

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

TEST_F(InterfacesTest, ConstructOnlyLocalLoopInterface)
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

TEST_F(InterfacesTest, GetAllInterfaces)
{
    Interfaces interfaces;
    std::string localLoopbackInterface;
    ASSERT_TRUE(interfaces.HaveLocalLoopbackInterface(localLoopbackInterface));
    auto allInterfaces = interfaces.GetAllInterfaces();
    EXPECT_TRUE(allInterfaces.find(localLoopbackInterface) != allInterfaces.end());
}

TEST_F(InterfacesTest, GetLocalLoopbackInterfaces)
{
    Interfaces interfaces;
    std::string localLoopbackInterface;
    ASSERT_TRUE(interfaces.HaveLocalLoopbackInterface(localLoopbackInterface));
    auto localLoopbackInterfaces = interfaces.GetLocalLoopbackInterfaces();
    EXPECT_TRUE(localLoopbackInterfaces.find(localLoopbackInterface) != localLoopbackInterfaces.end());
}

TEST_F(InterfacesTest, GetIPV4Interfaces)
{
    Interfaces interfaces;
    std::string localLoopbackInterface;
    ASSERT_TRUE(interfaces.HaveLocalLoopbackInterface(localLoopbackInterface));
    auto ipV4Interfaces = interfaces.GetIPV4Interfaces();
    EXPECT_TRUE(ipV4Interfaces.find(localLoopbackInterface) != ipV4Interfaces.end());
}

TEST_F(InterfacesTest, GetIPV6Interfaces)
{
    Interfaces interfaces;
    std::string localLoopbackInterface;
    ASSERT_TRUE(interfaces.HaveLocalLoopbackInterface(localLoopbackInterface));
    auto ipV6Interfaces = interfaces.GetIPV6Interfaces();
    EXPECT_TRUE(ipV6Interfaces.find(localLoopbackInterface) != ipV6Interfaces.end());
}

} // namespace network
