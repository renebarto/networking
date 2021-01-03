#include "GoogleTest.h"

#include "network-osal/AddressTuple.h"

namespace network {

TEST(AddressTupleTest, ConstructDefault)
{
    AddressTuple tuple;

    EXPECT_FALSE(tuple.IsValid());
    EXPECT_FALSE(tuple.IsMACAddress());
    EXPECT_FALSE(tuple.IsIPV4Address());
    EXPECT_FALSE(tuple.IsIPV6Address());
    EXPECT_EQ(MACAddress(), tuple.GetMACAddress());
    EXPECT_EQ(0, tuple.GetInterfaceIndex());
    EXPECT_EQ(IPV4Address::None, tuple.GetIPV4Address());
    EXPECT_EQ(IPV6Address::None, tuple.GetIPV6Address());
}

TEST(AddressTupleTest, ConstructMAC)
{
    MACAddress address({1, 2, 3, 4, 5, 6});
    int interfaceIndex = 123;
    AddressTuple tuple(address, interfaceIndex);

    EXPECT_TRUE(tuple.IsValid());
    EXPECT_TRUE(tuple.IsMACAddress());
    EXPECT_FALSE(tuple.IsIPV4Address());
    EXPECT_FALSE(tuple.IsIPV6Address());
    EXPECT_EQ(address, tuple.GetMACAddress());
    EXPECT_EQ(interfaceIndex, tuple.GetInterfaceIndex());
    EXPECT_EQ(IPV4Address::None, tuple.GetIPV4Address());
    EXPECT_EQ(IPV6Address::None, tuple.GetIPV6Address());
}

TEST(AddressTupleTest, ConstructIPV4)
{
    IPV4Address address(IPV4Address::LocalHost);
    AddressTuple tuple(address);

    EXPECT_TRUE(tuple.IsValid());
    EXPECT_FALSE(tuple.IsMACAddress());
    EXPECT_TRUE(tuple.IsIPV4Address());
    EXPECT_FALSE(tuple.IsIPV6Address());
    EXPECT_EQ(MACAddress(), tuple.GetMACAddress());
    EXPECT_EQ(0, tuple.GetInterfaceIndex());
    EXPECT_EQ(address, tuple.GetIPV4Address());
    EXPECT_EQ(IPV6Address::None, tuple.GetIPV6Address());
}

TEST(AddressTupleTest, ConstructIPV6)
{
    IPV6Address address(IPV6Address::LocalHost);
    AddressTuple tuple(address);

    EXPECT_TRUE(tuple.IsValid());
    EXPECT_FALSE(tuple.IsMACAddress());
    EXPECT_FALSE(tuple.IsIPV4Address());
    EXPECT_TRUE(tuple.IsIPV6Address());
    EXPECT_EQ(MACAddress(), tuple.GetMACAddress());
    EXPECT_EQ(0, tuple.GetInterfaceIndex());
    EXPECT_EQ(IPV4Address::None, tuple.GetIPV4Address());
    EXPECT_EQ(address, tuple.GetIPV6Address());
}

} // namespace network