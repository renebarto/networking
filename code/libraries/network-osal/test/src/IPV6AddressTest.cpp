#include "GoogleTest.h"

#include "network-osal/IPV6Address.h"
#include "network-osal/SocketDefinitions.h"
#include "utility/Deserialization.h"
#include "utility/Serialization.h"

namespace network {

TEST(IPV6Address, None)
{
    EXPECT_EQ(IPV6Address({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}), IPV6Address::None);
    EXPECT_EQ("::", serialization::Serialize(IPV6Address::None, 0));
}

TEST(IPV6Address, Any)
{
    EXPECT_EQ(IPV6Address({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}), IPV6Address::Any);
    EXPECT_EQ("::", serialization::Serialize(IPV6Address::Any, 0));
}

TEST(IPV6Address, Broadcast)
{
    EXPECT_EQ(IPV6Address({255, 255, 255, 255, 255, 255, 255, 255,
                           255, 255, 255, 255, 255, 255, 255, 255}), IPV6Address::Broadcast);
    EXPECT_EQ("ffff:ffff:ffff:ffff:ffff:ffff:ffff:ffff", serialization::Serialize(IPV6Address::Broadcast, 0));
}

TEST(IPV6Address, LocalHost)
{
    EXPECT_EQ(IPV6Address({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}), IPV6Address::LocalHost);
    EXPECT_EQ("::1", serialization::Serialize(IPV6Address::LocalHost, 0));
}

TEST(IPV6Address, Construct)
{
    IPV6Address address;

    EXPECT_EQ(IPV6Address({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}), address);
    EXPECT_EQ("::", serialization::Serialize(address, 0));
}

TEST(IPV6Address, ConstructCopy)
{
    IPV6Address address(IPV6Address::LocalHost);

    EXPECT_EQ(IPV6Address({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}), address);
    EXPECT_EQ("::1", serialization::Serialize(address, 0));
}

TEST(IPV6Address, ConstructFromInitializer)
{
    IPV6Address address({1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16});

    EXPECT_EQ(IPV6Address({1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16}), address);
    EXPECT_EQ("102:304:506:708:90a:b0c:d0e:f10", serialization::Serialize(address, 0));
}

TEST(IPV6Address, ConstructFromAddressType)
{
    IPV6Address address(IPV6Address::AddressType {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16});

    EXPECT_EQ(IPV6Address({1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16}), address);
    EXPECT_EQ("102:304:506:708:90a:b0c:d0e:f10", serialization::Serialize(address, 0));
}

TEST(IPV6Address, ConstructFromStruct)
{
    in6_addr addr;
    std::uint8_t bytes[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
    std::copy(std::begin(bytes), std::end(bytes), std::begin(addr.s6_addr));
    IPV6Address address(addr);

    EXPECT_EQ(IPV6Address({1, 2, 3, 4, 5, 6, 7, 8, 
                          9, 10, 11, 12, 13, 14, 15, 16}), address);
    EXPECT_EQ("102:304:506:708:90a:b0c:d0e:f10", serialization::Serialize(address, 0));
}

TEST(IPV6Address, Assign)
{
    IPV6Address address;
    address = IPV6Address::LocalHost;

    EXPECT_EQ(IPV6Address({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}), address);
    EXPECT_EQ("::1", serialization::Serialize(address, 0));
}

TEST(IPV6AddressTest, ParseNumeric)
{
    const std::string text = "fffe:fdfc:fbfa:f9f8:f7f6:f5f4:f3f2:f1f0";
    IPV6Address expected({255, 254, 253, 252, 251, 250, 249, 248, 247, 246, 245, 244, 243, 242, 241, 240});
    const IPV6Address & actual = IPV6Address::Parse(text);
    EXPECT_EQ(expected, actual);
}

TEST(IPV6AddressTest, ParseHostName)
{
    const std::string text = "localhost";

    EXPECT_EQ(IPV6Address({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}), IPV6Address::Parse(text));
}

TEST(IPV6AddressTest, ParseLocalhost)
{
    const std::string text = "::1";

    EXPECT_EQ(IPV6Address({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}), IPV6Address::Parse(text));
}

TEST(IPV6AddressTest, ParseBroadcast)
{
    const std::string text = "ff02::1";

    EXPECT_EQ(IPV6Address({255, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1}), IPV6Address::Parse(text));
}

TEST(IPV6AddressTest, ParseShortened)
{
    const std::string text = "FD3D:4056:8978:0:E133:19D7:2C09:290E";

    IPV6Address address = IPV6Address::Parse(text);
    EXPECT_EQ(IPV6Address({0xFD, 0x3D, 0x40, 0x56, 0x89, 0x78, 0x00, 0x00, 0xE1, 0x33, 0x19, 0xD7, 0x2C, 0x09, 0x29, 0x0E}),
              address);
}

TEST(IPV6AddressTest, ParseInvalid)
{
    const std::string text1 = "ffffe:fdfc:fbfa:f9f8:f7f6:f5f4:f3f2:f1f0";
    const std::string text2 = "fffe:fdfc:fbfa:f9f8:f7f6:f5f4:f3f2:f1f0:0";

    EXPECT_THROW(IPV6Address::Parse(text1), std::runtime_error);
    EXPECT_THROW(IPV6Address::Parse(text2), std::runtime_error);
}

TEST(IPV6AddressTest, TryParse)
{
    const std::string text = "fffe:fdfc:fbfa:f9f8:f7f6:f5f4:f3f2:f1f0";
    IPV6Address actual;
    IPV6Address expected({255, 254, 253, 252, 251, 250, 249, 248, 247, 246, 245, 244, 243, 242, 241, 240});
    EXPECT_TRUE(IPV6Address::TryParse(text, actual));
    EXPECT_EQ(expected, actual);
}

TEST(IPV6AddressTest, TryParseInvalid)
{
    const std::string text1 = "ffffe:fdfc:fbfa:f9f8:f7f6:f5f4:f3f2:f1f0";
    const std::string text2 = "fffe:fdfc:fbfa:f9f8:f7f6:f5f4:f3f2:f1f0:0";
    IPV6Address ipAddress;
    EXPECT_FALSE(IPV6Address::TryParse(text1, ipAddress));
    EXPECT_FALSE(IPV6Address::TryParse(text2, ipAddress));
}

TEST(IPV6AddressTest, ConvertAddress)
{
    IPV6Address ipAddress({255, 254, 253, 252, 251, 250, 249, 248, 247, 246, 245, 244, 243, 242, 241, 240});
    in6_addr actual;
    in6_addr expected;
    std::uint8_t address[] {255, 254, 253, 252, 251, 250, 249, 248, 247, 246, 245, 244, 243, 242, 241, 240};
    std::copy(std::begin(address), std::end(address), std::begin(expected.s6_addr));
    actual = ipAddress.ConvertAddress();
    EXPECT_TRUE(std::equal(std::begin(expected.s6_addr), std::end(expected.s6_addr), std::begin(actual.s6_addr)));
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

TEST(IPV6AddressTest, StreamExtraction)
{
    std::string text = "102:304:506:708:90a:b0c:d0e:f10";
    IPV6Address expected({1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16});
    IPV6Address actual;
    std::istringstream stream(text);
    stream >> actual;
    EXPECT_FALSE(stream.bad());
    EXPECT_EQ(expected, actual);
}

TEST(IPV6AddressTest, StreamInsertion)
{
    IPV6Address ipAddress({1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16});
    std::string expected = "102:304:506:708:90a:b0c:d0e:f10";
    std::ostringstream stream;
    stream << ipAddress;
    EXPECT_EQ(expected, stream.str());
}

TEST(IPV6AddressTest, Deserialize)
{
    std::string text = "102:304:506:708:90a:b0c:d0e:f10";
    IPV6Address expected({1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16});
    IPV6Address actual;
    EXPECT_TRUE(serialization::Deserialize(text, actual));
    EXPECT_EQ(expected, actual);
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
