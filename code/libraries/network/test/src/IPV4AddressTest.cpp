#include "GoogleTest.h"

#include "network/IPV4Address.h"
#include "utility/Serialization.h"

namespace network {

TEST(IPV4Address, None)
{
  EXPECT_EQ(IPV4Address({0, 0, 0, 0}), IPV4Address::None.Address());
  EXPECT_EQ(std::uint32_t {0x00000000}, IPV4Address::Any.GetUInt32());
  EXPECT_EQ("0.0.0.0", serialization::Serialize(IPV4Address::None, 0));
}

TEST(IPV4Address, Any)
{
  EXPECT_EQ(IPV4Address({0, 0, 0, 0}), IPV4Address::Any.Address());
  EXPECT_EQ(std::uint32_t {0x00000000}, IPV4Address::Any.GetUInt32());
  EXPECT_EQ("0.0.0.0", serialization::Serialize(IPV4Address::Any, 0));
}

TEST(IPV4Address, Broadcast)
{
  EXPECT_EQ(IPV4Address({255, 255, 255, 255}), IPV4Address::Broadcast.Address());
  EXPECT_EQ(std::uint32_t {0xFFFFFFFF}, IPV4Address::Broadcast.GetUInt32());
  EXPECT_EQ("255.255.255.255", serialization::Serialize(IPV4Address::Broadcast, 0));
}

TEST(IPV4Address, LocalHost)
{
  EXPECT_EQ(IPV4Address({127, 0, 0, 1}), IPV4Address::LocalHost.Address());
  EXPECT_EQ(std::uint32_t {0x7F000001}, IPV4Address::LocalHost.GetUInt32());
  EXPECT_EQ("127.0.0.1", serialization::Serialize(IPV4Address::LocalHost, 0));
}

TEST(IPV4Address, Construct)
{
  IPV4Address address;

  EXPECT_EQ(IPV4Address({0, 0, 0, 0}), address.Address());
  EXPECT_EQ(std::uint32_t {0x00000000}, address.GetUInt32());
  EXPECT_EQ("0.0.0.0", serialization::Serialize(address, 0));
}

TEST(IPV4Address, ConstructFromBytes)
{
  IPV4Address address({1, 2, 3, 4});

  EXPECT_EQ(IPV4Address({1, 2, 3, 4}), address.Address());
  EXPECT_EQ(std::uint32_t {0x01020304}, address.GetUInt32());
  EXPECT_EQ("1.2.3.4", serialization::Serialize(address, 0));
}

TEST(IPV4Address, ConstructFromUInt32)
{
  IPV4Address address(0x12345678);

  EXPECT_EQ(IPV4Address({0x12, 0x34, 0x56, 0x78}), address.Address());
  EXPECT_EQ(std::uint32_t {0x12345678}, address.GetUInt32());
  EXPECT_EQ("18.52.86.120", serialization::Serialize(address, 0));
}

TEST(IPV4AddressTest, OperatorEqual)
{
    IPV4Address::AddressType ipAddress = {1, 2, 3, 4};
    IPV4Address target(ipAddress);
    IPV4Address ref1;
    IPV4Address ref2({0, 0, 0, 0});
    IPV4Address ref3(ipAddress);
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

TEST(IPV4AddressTest, OperatorNotEqual)
{
    IPV4Address::AddressType ipAddress = {1, 2, 3, 4};
    IPV4Address target(ipAddress);
    IPV4Address ref1;
    IPV4Address ref2({0, 0, 0, 0});
    IPV4Address ref3(ipAddress);
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

TEST(IPV4AddressTest, StreamInsertion)
{
    IPV4Address ipAddress({1, 2, 3, 4});
    const std::string expected = "1.2.3.4";
    std::ostringstream stream;
    stream << ipAddress;
    EXPECT_EQ(expected, stream.str());
}

TEST(IPV4AddressTest, Serialize)
{
    IPV4Address ipAddress({1, 2, 3, 4});
    EXPECT_EQ("1.2.3.4", serialization::Serialize(ipAddress, 0));
}

} // namespace network
