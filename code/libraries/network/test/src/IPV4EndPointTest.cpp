#include "GoogleTest.h"

#include "utility/Deserialization.h"
#include "utility/Serialization.h"
#include "network/IPV4EndPoint.h"

namespace network {

TEST(IPV4EndPointTest, Constructor)
{
    IPV4EndPoint target;
    IPV4Address referenceAddress;
    uint16_t referencePort = AnyPort;
    const std::string expected = "0.0.0.0:0";
    EXPECT_EQ(referenceAddress, target.IPAddress());
    EXPECT_EQ(referencePort, target.Port());
    EXPECT_EQ(expected, serialization::Serialize(target, 0));
}

TEST(IPV4EndPointTest, ConstructorCopy)
{
    IPV4EndPoint ipEndPoint(IPV4Address({ 1, 2, 3, 4 }), 1234);
    IPV4EndPoint target(ipEndPoint);
    const std::string expected = "1.2.3.4:1234";
    EXPECT_EQ(ipEndPoint.IPAddress(), target.IPAddress());
    EXPECT_EQ(ipEndPoint.Port(), target.Port());
    EXPECT_EQ(expected, serialization::Serialize(target, 0));
}

TEST(IPV4EndPointTest, ConstructorIPAddress)
{
    IPV4Address ipAddress({ 1, 2, 3, 4 });
    IPV4EndPoint target(ipAddress);
    const std::string expected = "1.2.3.4:0";
    EXPECT_EQ(ipAddress, target.IPAddress());
    EXPECT_EQ(AnyPort, target.Port());
    EXPECT_EQ(expected, serialization::Serialize(target, 0));
}

TEST(IPV4EndPointTest, ConstructorIPAddressPort)
{
    IPV4Address ipAddress({ 1, 2, 3, 4 });
    uint16_t port = 1234;
    IPV4EndPoint target(ipAddress, port);
    const std::string expected = "1.2.3.4:1234";
    EXPECT_EQ(ipAddress, target.IPAddress());
    EXPECT_EQ(port, target.Port());
    EXPECT_EQ(expected, serialization::Serialize(target, 0));
}

TEST(IPV4EndPointTest, ConstructorByteArray)
{
    IPV4Address::AddressType address { 1, 2, 3, 4 };
    IPV4EndPoint target(address);
    const std::string expected = "1.2.3.4:0";
    EXPECT_EQ(address, target.IPAddress().Address());
    EXPECT_EQ(AnyPort, target.Port());
    EXPECT_EQ(expected, serialization::Serialize(target, 0));
}

TEST(IPV4EndPointTest, ConstructorByteArrayPort)
{
    IPV4Address::AddressType address { 1, 2, 3, 4 };
    uint16_t port = 1234;
    IPV4EndPoint target(address, port);
    const std::string expected = "1.2.3.4:1234";
    EXPECT_EQ(address, target.IPAddress().Address());
    EXPECT_EQ(port, target.Port());
    EXPECT_EQ(expected, serialization::Serialize(target, 0));
}

TEST(IPV4EndPointTest, ConstructorInAddr)
{
#if defined(PLATFORM_LINUX)
    in_addr ipAddress = { 0x04030201 };
#else
    in_addr ipAddress = { 1, 2, 3, 4 };
#endif
    IPV4EndPoint target(ipAddress);
    const std::string expected = "1.2.3.4:0";
    EXPECT_EQ(IPV4Address::AddressType({ 1, 2, 3, 4 }), target.IPAddress().Address());
    EXPECT_EQ(AnyPort, target.Port());
    EXPECT_EQ(expected, serialization::Serialize(target, 0));
}

TEST(IPV4EndPointTest, ConstructorInAddrPort)
{
#if defined(PLATFORM_LINUX)
    in_addr ipAddress = { 0x04030201 };
#else
    in_addr ipAddress = { 1, 2, 3, 4 };
#endif
    uint16_t port = 1234;
    IPV4EndPoint target(ipAddress, port);
    const std::string expected = "1.2.3.4:1234";
    EXPECT_EQ(IPV4Address::AddressType({ 1, 2, 3, 4 }), target.IPAddress().Address());
    EXPECT_EQ(port, target.Port());
    EXPECT_EQ(expected, serialization::Serialize(target, 0));
}

TEST(IPV4EndPointTest, ConstructorIPAddressStructPort)
{
#if defined(PLATFORM_LINUX)
    in_addr ipAddress = { 0x04030201 };
#else
    in_addr ipAddress = { 1, 2, 3, 4 };
#endif
    uint16_t port = 1234;
    IPV4EndPoint target(ipAddress, port);
    const std::string expected = "1.2.3.4:1234";
    EXPECT_EQ(ipAddress.s_addr, target.IPAddress().GetUInt32());
    EXPECT_EQ(port, target.Port());
    EXPECT_EQ(expected, serialization::Serialize(target, 0));
}

TEST(IPV4EndPointTest, ConstructorPort)
{
    uint16_t port = 1234;
    IPV4EndPoint target(port);
    const std::string expected = "0.0.0.0:1234";
    EXPECT_EQ(IPV4Address::None, target.IPAddress());
    EXPECT_EQ(port, target.Port());
    EXPECT_EQ(expected, serialization::Serialize(target, 0));
}

TEST(IPV4EndPointTest, ConstructorSockAddr)
{
#if defined(PLATFORM_LINUX)
    SockAddrIPV4 address({ 0x04030201 }, 1234);
#else
    SockAddrIPV4 address({ 1, 2, 3, 4 }, 1234);
#endif
    IPV4EndPoint target(address);
    const std::string expected = "1.2.3.4:1234";
    EXPECT_EQ(IPV4Address::AddressType({ 1, 2, 3, 4 }), target.IPAddress().Address());
    EXPECT_EQ(address.sin_port, target.Port());
    EXPECT_EQ(expected, serialization::Serialize(target, 0));
}

TEST(IPV4EndPointTest, Assign)
{
    IPV4EndPoint ipEndPoint(IPV4Address(IPV4Address::AddressType { 1, 2, 3, 4 }), 1234);
    IPV4EndPoint target;
    target = ipEndPoint;
    const std::string expected = "1.2.3.4:1234";
    EXPECT_EQ(ipEndPoint.IPAddress(), target.IPAddress());
    EXPECT_EQ(ipEndPoint.Port(), target.Port());
    EXPECT_EQ(expected, serialization::Serialize(target, 0));
}

TEST(IPV4EndPointTest, Parse)
{
    const std::string text = "255.254.253.252:12345";
    IPV4Address ipAddress({ 255, 254, 253, 252 });
    uint16_t port = 12345;
    IPV4EndPoint expected(ipAddress, port);
    const IPV4EndPoint & actual = IPV4EndPoint::Parse(text);
    EXPECT_EQ(expected, actual);
}

TEST(IPV4EndPointTest, ParseInvalid)
{
    const std::string text1 = "256.255.255.255:12345";
    const std::string text2 = "255.255.255.255.255";
    const std::string text3 = "255.255.255.255:123456";

    EXPECT_THROW(IPV4EndPoint::Parse(text1), std::runtime_error);
    EXPECT_THROW(IPV4EndPoint::Parse(text2), std::runtime_error);
    EXPECT_THROW(IPV4EndPoint::Parse(text3), std::runtime_error);
}

TEST(IPV4EndPointTest, TryParseIPAddressOnly)
{
    const std::string text = "255.254.253.252";
    IPV4Address ipAddress({ 255, 254, 253, 252 });
    IPV4EndPoint expected(ipAddress);
    IPV4EndPoint actual;
    EXPECT_TRUE(IPV4EndPoint::TryParse(text, actual));
    EXPECT_EQ(expected, actual);
}

TEST(IPV4EndPointTest, TryParse)
{
    const std::string text = "255.254.253.252:8080";
    IPV4Address ipAddress({ 255, 254, 253, 252 });
    uint16_t port = 8080;
    IPV4EndPoint actual;
    IPV4EndPoint expected(ipAddress, port);
    EXPECT_TRUE(IPV4EndPoint::TryParse(text, actual));
    EXPECT_EQ(expected, actual);
}

TEST(IPV4EndPointTest, TryParseInvalid)
{
    const std::string text1 = "256.255.255.255:12345";
    const std::string text2 = "255.255.255.255.255";
    const std::string text3 = "255.255.255.255:123456";
    IPV4EndPoint ipAddress;
    EXPECT_FALSE(IPV4EndPoint::TryParse(text1, ipAddress));
    EXPECT_FALSE(IPV4EndPoint::TryParse(text2, ipAddress));
    EXPECT_FALSE(IPV4EndPoint::TryParse(text3, ipAddress));
}

TEST(IPV4EndPointTest, OperatorEqual)
{
    IPV4Address ipAddress(IPV4Address::AddressType { 1, 2, 3, 4 });
    uint16_t port = 1234;
    IPV4EndPoint target(ipAddress, port);
    IPV4EndPoint ref1;
    IPV4EndPoint ref2(ipAddress);
    IPV4EndPoint ref3(ipAddress, port);
    IPV4EndPoint ref4(port);
    EXPECT_FALSE(target == ref1);
    EXPECT_FALSE(target == ref2);
    EXPECT_TRUE(target == ref3);
    EXPECT_FALSE(target == ref4);
    EXPECT_FALSE(ref1 == target);
    EXPECT_FALSE(ref2 == target);
    EXPECT_TRUE(ref3 == target);
    EXPECT_FALSE(ref4 == target);
}

TEST(IPV4EndPointTest, OperatorNotEqual)
{
    IPV4Address ipAddress(IPV4Address::AddressType { 1, 2, 3, 4 });
    uint16_t port = 1234;
    IPV4EndPoint target(ipAddress, port);
    IPV4EndPoint ref1;
    IPV4EndPoint ref2(ipAddress);
    IPV4EndPoint ref3(ipAddress, port);
    IPV4EndPoint ref4(port);
    EXPECT_TRUE(target != ref1);
    EXPECT_TRUE(target != ref2);
    EXPECT_FALSE(target != ref3);
    EXPECT_TRUE(target != ref4);
    EXPECT_TRUE(ref1 != target);
    EXPECT_TRUE(ref2 != target);
    EXPECT_FALSE(ref3 != target);
    EXPECT_TRUE(ref4 != target);
}

TEST(IPV4EndPointTest, StreamExtraction)
{
    const std::string text = "1.2.3.4:1234";
    IPV4EndPoint expected(IPV4Address({1, 2, 3, 4}), 1234);
    IPV4EndPoint actual;
    std::istringstream stream(text);
    stream >> actual;
    EXPECT_FALSE(stream.bad());
    EXPECT_EQ(expected, actual);
}

TEST(IPV4EndPointTest, StreamInsertion)
{
    IPV4EndPoint endPoint(IPV4Address({1, 2, 3, 4}), 1234);
    std::string expected = "1.2.3.4:1234";
    std::ostringstream stream;
    stream << endPoint;
    EXPECT_EQ(expected, stream.str());
}

TEST(IPV4EndPointTest, Deserialize)
{
    IPV4EndPoint expected(IPV4Address({1, 2, 3, 4}), 1234);
    IPV4EndPoint actual;
    EXPECT_TRUE(serialization::Deserialize("1.2.3.4:1234", actual));
    EXPECT_EQ(expected, actual);
}

TEST(IPV4EndPointTest, Serialize)
{
    const std::string expected = "1.2.3.4:1234";
    IPV4EndPoint endPoint(IPV4Address({1, 2, 3, 4}), 1234);
    auto actual = serialization::Serialize(endPoint, 0);
    EXPECT_EQ(expected, actual);
}

} // namespace network
