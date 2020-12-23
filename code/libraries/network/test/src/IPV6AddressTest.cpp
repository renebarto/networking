#include "GoogleTest.h"

#include "network/IPV6Address.h"
#include "utility/Serialization.h"

namespace network {

TEST(IPV6Address, None)
{
  EXPECT_EQ(IPV6Address({0, 0, 0, 0, 0, 0, 0, 0,
                         0, 0, 0, 0, 0, 0, 0, 0}), IPV6Address::None.Address());
  EXPECT_EQ("::", serialization::Serialize(IPV6Address::None, 0));
}

TEST(IPV6Address, Any)
{
  EXPECT_EQ(IPV6Address({0, 0, 0, 0, 0, 0, 0, 0,
                         0, 0, 0, 0, 0, 0, 0, 0}), IPV6Address::Any.Address());
  EXPECT_EQ("::", serialization::Serialize(IPV6Address::Any, 0));
}

TEST(IPV6Address, Broadcast)
{
  EXPECT_EQ(IPV6Address({255, 255, 255, 255, 255, 255, 255, 255,
                         255, 255, 255, 255, 255, 255, 255, 255}), IPV6Address::Broadcast.Address());
  EXPECT_EQ("ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff", serialization::Serialize(IPV6Address::Broadcast, 0));
}

TEST(IPV6Address, LocalHost)
{
  EXPECT_EQ(IPV6Address({0, 0, 0, 0, 0, 0, 0, 0, 
                         0, 0, 0, 0, 0, 0, 0, 1}), IPV6Address::LocalHost.Address());
  EXPECT_EQ("::1", serialization::Serialize(IPV6Address::LocalHost, 0));
}

TEST(IPV6Address, Construct)
{
  IPV6Address address;

  EXPECT_EQ(IPV6Address({0, 0, 0, 0, 0, 0, 0, 0,
                         0, 0, 0, 0, 0, 0, 0, 0}), address.Address());
  EXPECT_EQ("::", serialization::Serialize(address, 0));
}

TEST(IPV6Address, ConstructFromBytes)
{
  IPV6Address address({1, 2, 3, 4, 5, 6, 7, 8, 
                       9, 10, 11, 12, 13, 14, 15, 16});

  EXPECT_EQ(IPV6Address({1, 2, 3, 4, 5, 6, 7, 8, 
                         9, 10, 11, 12, 13, 14, 15, 16}), address.Address());
  EXPECT_EQ("102:304:506:708:90a:b0c:d0e:f10", serialization::Serialize(address, 0));
}

TEST(IPV6AddressTest, OperatorEqual)
{
    IPV6Address::AddressType ipAddress {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    IPV6Address target(ipAddress);
    IPV6Address ref1;
    IPV6Address ref2({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0});
    IPV6Address ref3(ipAddress);
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

TEST(IPV6AddressTest, OperatorNotEqual)
{
    IPV6Address::AddressType ipAddress {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16};
    IPV6Address target(ipAddress);
    IPV6Address ref1;
    IPV6Address ref2({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0});
    IPV6Address ref3(ipAddress);
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

TEST(IPV6AddressTest, StreamInsertion)
{
    IPV6Address ipAddress({1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16});
    std::string expected = "102:304:506:708:90a:b0c:d0e:f10";
    std::ostringstream stream;
    stream << ipAddress;
    EXPECT_EQ(expected, stream.str());
}

TEST(IPV6AddressTest, Serialize)
{
    IPV6Address ipAddress({1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16});
    EXPECT_EQ("102:304:506:708:90a:b0c:d0e:f10", serialization::Serialize(ipAddress, 0));
}

TEST(IPV6AddressTest, StreamInsertionAddressUnspecified)
{
    IPV6Address ipAddress({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0});
    std::ostringstream stream;
    stream << ipAddress;
    EXPECT_EQ("::", stream.str());
}

TEST(IPV6AddressTest, StreamInsertionLocalhostAddress)
{
    IPV6Address ipAddress({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1});
    std::ostringstream stream;
    stream << ipAddress;
    EXPECT_EQ("::1", stream.str());
}

TEST(IPV6AddressTest, StreamInsertionAddressTrailingZeroSequence)
{
    IPV6Address ipAddress({1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0});
    std::ostringstream stream;
    stream << ipAddress;
    EXPECT_EQ("100::", stream.str());
}

TEST(IPV6AddressTest, StreamInsertionMultipleZeroSequences1)
{
    IPV6Address ipAddress({1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1});
    std::ostringstream stream;
    stream << ipAddress;
    EXPECT_EQ("100:0:100::1", stream.str());
}

TEST(IPV6AddressTest, StreamInsertionMultipleZeroSequences2)
{
    IPV6Address ipAddress({1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1});
    std::ostringstream stream;
    stream << ipAddress;
    EXPECT_EQ("100::100:0:0:1", stream.str());
}

TEST(IPV6AddressTest, StreamInsertionShortened)
{
    IPV6Address ipAddress({0xFE, 0x3D, 0x40, 0x56, 0x89, 0x78, 0x00, 0x00, 0xE1, 0x33, 0x19, 0xD7, 0x2C, 0x09, 0x29, 0x0E});
    std::ostringstream stream;
    stream << ipAddress;
    EXPECT_EQ("fe3d:4056:8978:0:e133:19d7:2c09:290e", stream.str());
}

} // namespace network
