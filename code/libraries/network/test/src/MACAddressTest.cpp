#include "GoogleTest.h"

#include "network/MACAddress.h"
#include "utility/Serialization.h"

namespace network {

TEST(MACAddress, Construct)
{
  MACAddress address;

  EXPECT_EQ(MACAddress::AddressType({0, 0, 0, 0}), address.Address());
  EXPECT_EQ(0, address.InterfaceIndex());
  EXPECT_EQ("00-00-00-00-00-00", serialization::Serialize(address, 0));
}

TEST(MACAddress, ConstructFromBytes)
{
  MACAddress address({1, 2, 3, 4, 5, 6}, 123);

  EXPECT_EQ(MACAddress::AddressType({1, 2, 3, 4, 5, 6}), address.Address());
  EXPECT_EQ(123, address.InterfaceIndex());
  EXPECT_EQ("01-02-03-04-05-06", serialization::Serialize(address, 0));
}

TEST(MACAddressTest, OperatorEqualAddress)
{
    MACAddress::AddressType ipAddress {1, 2, 3, 4, 5, 6};
    MACAddress::AddressType target(ipAddress);
    MACAddress::AddressType ref1 {};
    MACAddress::AddressType ref2 {0, 0, 0, 0, 0, 0};
    MACAddress::AddressType ref3(ipAddress);
    EXPECT_FALSE(target == ref1);
    EXPECT_FALSE(target == ref2);
    EXPECT_TRUE(target == ref3);
    EXPECT_FALSE(ref1 == target);
    EXPECT_TRUE(ref1 == ref2);
    EXPECT_FALSE(ref1 == ref3);
    EXPECT_FALSE(ref2 == target);
    EXPECT_TRUE(ref2 == ref1);
    EXPECT_FALSE(ref2 == ref3);
    EXPECT_TRUE(ref3 == target);
    EXPECT_FALSE(ref3 == ref1);
    EXPECT_FALSE(ref3 == ref2);
}

TEST(MACAddressTest, OperatorNotEqualAddress)
{
    MACAddress::AddressType ipAddress {1, 2, 3, 4, 5, 6};
    MACAddress::AddressType target(ipAddress);
    MACAddress::AddressType ref1 {};
    MACAddress::AddressType ref2 {0, 0, 0, 0, 0, 0};
    MACAddress::AddressType ref3(ipAddress);
    EXPECT_TRUE(target != ref1);
    EXPECT_TRUE(target != ref2);
    EXPECT_FALSE(target != ref3);
    EXPECT_TRUE(ref1 != target);
    EXPECT_FALSE(ref1 != ref2);
    EXPECT_TRUE(ref1 != ref3);
    EXPECT_TRUE(ref2 != target);
    EXPECT_FALSE(ref2 != ref1);
    EXPECT_TRUE(ref2 != ref3);
    EXPECT_FALSE(ref3 != target);
    EXPECT_TRUE(ref3 != ref1);
    EXPECT_TRUE(ref3 != ref2);
}

TEST(MACAddressTest, OperatorEqual)
{
    MACAddress::AddressType macAddress {1, 2, 3, 4, 5, 6};
    MACAddress target(macAddress, 0);
    MACAddress ref1;
    MACAddress ref2({0, 0, 0, 0, 0, 0}, 0);
    MACAddress ref3(macAddress, 2);
    EXPECT_FALSE(target == ref1);
    EXPECT_FALSE(target == ref2);
    EXPECT_FALSE(target == ref3);
    EXPECT_FALSE(ref1 == target);
    EXPECT_TRUE(ref1 == ref2);
    EXPECT_FALSE(ref1 == ref3);
    EXPECT_FALSE(ref2 == target);
    EXPECT_TRUE(ref2 == ref1);
    EXPECT_FALSE(ref2 == ref3);
    EXPECT_FALSE(ref3 == target);
    EXPECT_FALSE(ref3 == ref1);
    EXPECT_FALSE(ref3 == ref2);
}

TEST(MACAddressTest, OperatorNotEqual)
{
    MACAddress::AddressType macAddress {1, 2, 3, 4, 5, 6};
    MACAddress target(macAddress, 0);
    MACAddress ref1;
    MACAddress ref2({0, 0, 0, 0, 0, 0}, 0);
    MACAddress ref3(macAddress, 2);
    EXPECT_TRUE(target != ref1);
    EXPECT_TRUE(target != ref2);
    EXPECT_TRUE(target != ref3);
    EXPECT_TRUE(ref1 != target);
    EXPECT_FALSE(ref1 != ref2);
    EXPECT_TRUE(ref1 != ref3);
    EXPECT_TRUE(ref2 != target);
    EXPECT_FALSE(ref2 != ref1);
    EXPECT_TRUE(ref2 != ref3);
    EXPECT_TRUE(ref3 != target);
    EXPECT_TRUE(ref3 != ref1);
    EXPECT_TRUE(ref3 != ref2);
}

TEST(MACAddressTest, StreamInsertion)
{
    MACAddress target({1, 2, 3, 4, 5, 6}, 0);
    const std::string expected = "01-02-03-04-05-06";
    std::ostringstream stream;
    stream << target;
    EXPECT_EQ(expected, stream.str());
}

TEST(MACAddressTest, Serialize)
{
    MACAddress target({1, 2, 3, 4, 5, 6}, 0);
    const std::string expected = "01-02-03-04-05-06";
    EXPECT_EQ(expected, serialization::Serialize(target, 0));
}

} // namespace network
