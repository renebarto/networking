#include "GoogleTest.h"

#include "network-osal/IPV4Address.h"
#include "network-osal/SocketDefinitions.h"
#include "utility/Deserialization.h"
#include "utility/Serialization.h"

namespace network {

TEST(IPV4Address, None)
{
  EXPECT_EQ(IPV4Address({0, 0, 0, 0}), IPV4Address::None);
  EXPECT_EQ(std::uint32_t {0x00000000}, IPV4Address::Any.GetUInt32());
  EXPECT_EQ("0.0.0.0", serialization::Serialize(IPV4Address::None, 0));
}

TEST(IPV4Address, Any)
{
    EXPECT_EQ(IPV4Address({0, 0, 0, 0}), IPV4Address::Any);
    EXPECT_EQ(std::uint32_t {0x00000000}, IPV4Address::Any.GetUInt32());
    EXPECT_EQ("0.0.0.0", serialization::Serialize(IPV4Address::Any, 0));
}

TEST(IPV4Address, Broadcast)
{
    EXPECT_EQ(IPV4Address({255, 255, 255, 255}), IPV4Address::Broadcast);
    EXPECT_EQ(std::uint32_t {0xFFFFFFFF}, IPV4Address::Broadcast.GetUInt32());
    EXPECT_EQ("255.255.255.255", serialization::Serialize(IPV4Address::Broadcast, 0));
}

TEST(IPV4Address, LocalHost)
{
    EXPECT_EQ(IPV4Address({127, 0, 0, 1}), IPV4Address::LocalHost);
    EXPECT_EQ(std::uint32_t {0x0100007F}, IPV4Address::LocalHost.GetUInt32());
    EXPECT_EQ("127.0.0.1", serialization::Serialize(IPV4Address::LocalHost, 0));
}

TEST(IPV4Address, Construct)
{
    IPV4Address address;

    EXPECT_EQ(IPV4Address({0, 0, 0, 0}), address);
    EXPECT_EQ(std::uint32_t {0x00000000}, address.GetUInt32());
    EXPECT_EQ("0.0.0.0", serialization::Serialize(address, 0));
}

TEST(IPV4Address, ConstructCopy)
{
    IPV4Address address(IPV4Address::LocalHost);

    EXPECT_EQ(IPV4Address({127, 0, 0, 1}), address);
    EXPECT_EQ(std::uint32_t {0x0100007F}, address.GetUInt32());
    EXPECT_EQ("127.0.0.1", serialization::Serialize(address, 0));
}

TEST(IPV4Address, ConstructFromInitializer)
{
    IPV4Address address({1, 2, 3, 4});

    EXPECT_EQ(IPV4Address({1, 2, 3, 4}), address);
    EXPECT_EQ(std::uint32_t {0x04030201}, address.GetUInt32());
    EXPECT_EQ("1.2.3.4", serialization::Serialize(address, 0));
}

TEST(IPV4Address, ConstructFromAddressType)
{
    IPV4Address address(IPV4Address::AddressType {1, 2, 3, 4});

    EXPECT_EQ(IPV4Address(IPV4Address::AddressType {1, 2, 3, 4}), address);
    EXPECT_EQ(std::uint32_t {0x04030201}, address.GetUInt32());
    EXPECT_EQ("1.2.3.4", serialization::Serialize(address, 0));
}

TEST(IPV4Address, ConstructFromStruct)
{
    in_addr addr;
    addr.s_addr = 0xFF000102;
    IPV4Address address(addr);

    EXPECT_EQ(IPV4Address(IPV4Address::AddressType {0x02, 0x01, 0x00, 0xFF}), address);
    EXPECT_EQ(std::uint32_t {0xFF000102}, address.GetUInt32());
    EXPECT_EQ("2.1.0.255", serialization::Serialize(address, 0));
}

TEST(IPV4Address, Assign)
{
    IPV4Address address;
    address = IPV4Address::LocalHost;

    EXPECT_EQ(IPV4Address({127, 0, 0, 1}), address);
    EXPECT_EQ(std::uint32_t {0x0100007F}, address.GetUInt32());
    EXPECT_EQ("127.0.0.1", serialization::Serialize(address, 0));
}

TEST(IPV4AddressTest, ParseNumeric)
{
    const std::string text = "255.254.253.252";
    IPV4Address expected({255, 254, 253, 252});
    const IPV4Address & actual = IPV4Address::Parse(text);
    EXPECT_EQ(expected, actual);
}

TEST(IPV4AddressTest, ParseHostName)
{
    const std::string text = "localhost";
    EXPECT_EQ(IPV4Address({127, 0, 0, 1}), IPV4Address::Parse(text));
}

TEST(IPV4AddressTest, ParseBroadcast)
{
    const std::string text1 = "255.255.255.255";
    EXPECT_EQ(IPV4Address({255, 255, 255, 255}), IPV4Address::Parse(text1));
}

TEST(IPV4AddressTest, ParseInvalid)
{
    const std::string text1 = "256.255.255.255";
    const std::string text2 = "255.255.255.255.255";

    EXPECT_THROW(IPV4Address::Parse(text1), std::runtime_error);
    EXPECT_THROW(IPV4Address::Parse(text2), std::runtime_error);
}

TEST(IPV4AddressTest, TryParse)
{
    const std::string text = "255.254.253.252";
    IPV4Address::AddressType ipAddress {255, 254, 253, 252};
    IPV4Address actual;
    IPV4Address expected(ipAddress);
    EXPECT_TRUE(IPV4Address::TryParse(text, actual));
    EXPECT_EQ(expected, actual);
}

TEST(IPV4AddressTest, TryParseInvalid)
{
    const std::string text1 = "256.255.255.255";
    const std::string text2 = "255.255.255.255.255";
    IPV4Address ipAddress;
    EXPECT_FALSE(IPV4Address::TryParse(text1, ipAddress));
    EXPECT_FALSE(IPV4Address::TryParse(text2, ipAddress));
}

TEST(IPV4AddressTest, ConvertAddress)
{
    IPV4Address ipAddress({255, 254, 253, 252});
    in_addr actual;
    in_addr expected;
    expected.s_addr = 0xFCFDFEFF;
    actual = ipAddress.ConvertAddress();
    EXPECT_EQ(expected.s_addr, actual.s_addr);
}

TEST(IPV4AddressTest, OperatorEqual)
{
    IPV4Address::AddressType ipAddress = {1, 2, 3, 4};
    IPV4Address target(ipAddress);
    IPV4Address ref1;
    IPV4Address ref2(IPV4Address::AddressType {0, 0, 0, 0});
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
    IPV4Address ref2(IPV4Address::AddressType {0, 0, 0, 0});
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

TEST(IPV4AddressTest, StreamExtraction)
{
    const std::string text = "1.2.3.4";
    IPV4Address expected({1, 2, 3, 4});
    IPV4Address actual;
    std::istringstream stream(text);
    stream >> actual;
    EXPECT_FALSE(stream.bad());
    EXPECT_EQ(expected, actual);
}

TEST(IPV4AddressTest, StreamInsertion)
{
    IPV4Address ipAddress({1, 2, 3, 4});
    const std::string expected = "1.2.3.4";
    std::ostringstream stream;
    stream << ipAddress;
    EXPECT_EQ(expected, stream.str());
}

TEST(IPV4AddressTest, Deserialize)
{
    IPV4Address expected({1, 2, 3, 4});
    IPV4Address actual;
    EXPECT_TRUE(serialization::Deserialize("1.2.3.4", actual));
    EXPECT_EQ(expected, actual);
}

TEST(IPV4AddressTest, Serialize)
{
    const std::string expected = "1.2.3.4";
    IPV4Address ipAddress({1, 2, 3, 4});
    auto actual = serialization::Serialize(ipAddress, 0);
    EXPECT_EQ(expected, actual);
}

} // namespace network
