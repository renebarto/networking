#include "GoogleTest.h"

#include "utility/Deserialization.h"
#include "utility/Serialization.h"
#include "network/IPV6EndPoint.h"

namespace network {

TEST(IPV6EndPointTest, Constructor)
{
    IPV6EndPoint target;
    IPV6Address referenceAddress;
    uint16_t referencePort = AnyPort;
    const std::string expected = "::";
    EXPECT_EQ(referenceAddress, target.IPAddress());
    EXPECT_EQ(referencePort, target.Port());
    EXPECT_EQ(uint32_t{0}, target.FlowInfo());
    EXPECT_EQ(uint32_t{0}, target.ScopeID());
    EXPECT_EQ(expected, serialization::Serialize(target, 0));
}

TEST(IPV6EndPointTest, ConstructorCopy)
{
    IPV6EndPoint ipEndPoint(IPV6Address({ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 }), 1234);
    IPV6EndPoint target(ipEndPoint);
    const std::string expected = "[102:304:506:708:90a:b0c:d0e:f10]:1234";
    EXPECT_EQ(ipEndPoint.IPAddress(), target.IPAddress());
    EXPECT_EQ(ipEndPoint.Port(), target.Port());
    EXPECT_EQ(uint32_t{0}, target.FlowInfo());
    EXPECT_EQ(uint32_t{0}, target.ScopeID());
    EXPECT_EQ(expected, serialization::Serialize(target, 0));
}

TEST(IPV6EndPointTest, ConstructorIPAddress)
{
    IPV6Address ipAddress(IPV6Address::AddressType { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 });
    IPV6EndPoint target(ipAddress);
    const std::string expected = "102:304:506:708:90a:b0c:d0e:f10";
    EXPECT_EQ(ipAddress, target.IPAddress());
    // We compare directly here to not force creating storage for AnyPort
    EXPECT_TRUE(AnyPort == target.Port());
    EXPECT_EQ(uint32_t{0}, target.FlowInfo());
    EXPECT_EQ(uint32_t{0}, target.ScopeID());
    EXPECT_EQ(expected, serialization::Serialize(target, 0));
}

TEST(IPV6EndPointTest, ConstructorIPAddressPort)
{
    IPV6Address ipAddress(IPV6Address::AddressType { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 });
    uint16_t port = 1234;
    IPV6EndPoint target(ipAddress, port);
    const std::string expected = "[102:304:506:708:90a:b0c:d0e:f10]:1234";
    EXPECT_EQ(ipAddress, target.IPAddress());
    EXPECT_EQ(port, target.Port());
    EXPECT_EQ(uint32_t{0}, target.FlowInfo());
    EXPECT_EQ(uint32_t{0}, target.ScopeID());
    EXPECT_EQ(expected, serialization::Serialize(target, 0));
}

TEST(IPV6EndPointTest, ConstructorIPAddressPortFlowInfo)
{
    IPV6Address ipAddress(IPV6Address::AddressType { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 });
    uint16_t port = 1234;
    uint32_t flowInfo = 12345678;
    IPV6EndPoint target(ipAddress, port, flowInfo);
    const std::string expected = "[102:304:506:708:90a:b0c:d0e:f10]:1234";
    EXPECT_EQ(ipAddress, target.IPAddress());
    EXPECT_EQ(port, target.Port());
    EXPECT_EQ(flowInfo, target.FlowInfo());
    EXPECT_EQ(uint32_t{0}, target.ScopeID());
    EXPECT_EQ(expected, serialization::Serialize(target, 0));
}

TEST(IPV6EndPointTest, ConstructorIPAddressPortFlowInfoScopeID)
{
    IPV6Address ipAddress(IPV6Address::AddressType { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 });
    uint16_t port = 1234;
    uint32_t flowInfo = 12345678;
    uint32_t scopeID = 87654321;
    IPV6EndPoint target(ipAddress, port, flowInfo, scopeID);
    const std::string expected = "[102:304:506:708:90a:b0c:d0e:f10]:1234%87654321";
    EXPECT_EQ(ipAddress, target.IPAddress());
    EXPECT_EQ(port, target.Port());
    EXPECT_EQ(flowInfo, target.FlowInfo());
    EXPECT_EQ(scopeID, target.ScopeID());
    EXPECT_EQ(expected, serialization::Serialize(target, 0));
}

TEST(IPV6EndPointTest, ConstructorInAddr)
{
    in6_addr ipAddress = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
    IPV6EndPoint target(ipAddress);
    const std::string expected = "102:304:506:708:90a:b0c:d0e:f10";
    EXPECT_EQ(IPV6Address::AddressType({ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 }), target.IPAddress().Address());
    EXPECT_EQ(AnyPort, target.Port());
    EXPECT_EQ(expected, serialization::Serialize(target, 0));
}

TEST(IPV6EndPointTest, ConstructorInAddrPort)
{
    in6_addr ipAddress = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
    uint16_t port = 1234;
    IPV6EndPoint target(ipAddress, port);
    const std::string expected = "[102:304:506:708:90a:b0c:d0e:f10]:1234";
    EXPECT_EQ(IPV6Address::AddressType({ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 }), target.IPAddress().Address());
    EXPECT_EQ(port, target.Port());
    EXPECT_EQ(expected, serialization::Serialize(target, 0));
}

TEST(IPV6EndPointTest, ConstructorAddressTypePort)
{
    IPV6Address::AddressType ipAddress { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
    uint16_t port = 1234;
    IPV6EndPoint target(ipAddress, port);
    const std::string expected = "[102:304:506:708:90a:b0c:d0e:f10]:1234";
    EXPECT_EQ(IPV6Address::AddressType({ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 }), target.IPAddress().Address());
    EXPECT_EQ(port, target.Port());
    EXPECT_EQ(uint32_t{0}, target.FlowInfo());
    EXPECT_EQ(uint32_t{0}, target.ScopeID());
    EXPECT_EQ(expected, serialization::Serialize(target, 0));
}

TEST(IPV6EndPointTest, ConstructorAddressTypePortFlowInfo)
{
    IPV6Address::AddressType ipAddress { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
    uint16_t port = 1234;
    uint32_t flowInfo = 12345678;
    IPV6EndPoint target(ipAddress, port, flowInfo);
    const std::string expected = "[102:304:506:708:90a:b0c:d0e:f10]:1234";
    EXPECT_EQ(IPV6Address::AddressType({ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 }), target.IPAddress().Address());
    EXPECT_EQ(port, target.Port());
    EXPECT_EQ(flowInfo, target.FlowInfo());
    EXPECT_EQ(uint32_t{0}, target.ScopeID());
    EXPECT_EQ(expected, serialization::Serialize(target, 0));
}

TEST(IPV6EndPointTest, ConstructorAddressTypePortFlowInfoScopeID)
{
    IPV6Address::AddressType ipAddress { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };
    uint16_t port = 1234;
    uint32_t flowInfo = 12345678;
    uint32_t scopeID = 87654321;
    IPV6EndPoint target(ipAddress, port, flowInfo, scopeID);
    const std::string expected = "[102:304:506:708:90a:b0c:d0e:f10]:1234%87654321";
    EXPECT_EQ(IPV6Address::AddressType({ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 }), target.IPAddress().Address());
    EXPECT_EQ(port, target.Port());
    EXPECT_EQ(flowInfo, target.FlowInfo());
    EXPECT_EQ(scopeID, target.ScopeID());
    EXPECT_EQ(expected, serialization::Serialize(target, 0));
}

TEST(IPV6EndPointTest, ConstructorPort)
{
    uint16_t port = 1234;
    IPV6EndPoint target(port);
    const std::string expected = "[::]:1234";
    EXPECT_TRUE(IPV6Address::None == target.IPAddress());
    EXPECT_TRUE(port == target.Port());
    EXPECT_EQ(expected, serialization::Serialize(target, 0));
}

TEST(IPV6EndPointTest, ConstructorSockAddr)
{
    SockAddrIPV6 address({ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 }, 1234, 12345678, 87654321);
    IPV6EndPoint target(address);
    const std::string expected = "[102:304:506:708:90a:b0c:d0e:f10]:1234%87654321";
    EXPECT_EQ(IPV6Address::AddressType({ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 }), target.IPAddress().Address());
    EXPECT_EQ(address.sin6_port, target.Port());
    EXPECT_EQ(expected, serialization::Serialize(target, 0));
}

TEST(IPV6EndPointTest, Assign)
{
    IPV6EndPoint ipEndPoint(IPV6Address(IPV6Address::AddressType { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 }), 1234);
    IPV6EndPoint target;

    target = ipEndPoint;
    const std::string expected = "[102:304:506:708:90a:b0c:d0e:f10]:1234";
    EXPECT_EQ(ipEndPoint.IPAddress(), target.IPAddress());
    EXPECT_EQ(ipEndPoint.Port(), target.Port());
    EXPECT_EQ(uint32_t{0}, target.FlowInfo());
    EXPECT_EQ(uint32_t{0}, target.ScopeID());
    EXPECT_EQ(expected, serialization::Serialize(target, 0));
}

TEST(IPV6EndPointTest, ParseIPAddressOnly)
{
    const std::string text = "fffe:fdfc:fbfa:f9f8:f7f6:f5f4:f3f2:f1f0";
    IPV6Address ipAddress({ 255, 254, 253, 252, 251, 250, 249, 248, 247, 246, 245, 244, 243, 242, 241, 240 });
    IPV6EndPoint expected(ipAddress);
    const IPV6EndPoint & actual = IPV6EndPoint::Parse(text);
    EXPECT_EQ(expected, actual);
}

TEST(IPV6EndPointTest, Parse)
{
    const std::string text = "[fffe:fdfc:fbfa:f9f8:f7f6:f5f4:f3f2:f1f0]:12345";
    IPV6Address ipAddress({ 255, 254, 253, 252, 251, 250, 249, 248, 247, 246, 245, 244, 243, 242, 241, 240 });
    uint16_t port = 12345;
    IPV6EndPoint expected(ipAddress, port);
    const IPV6EndPoint & actual = IPV6EndPoint::Parse(text);
    EXPECT_EQ(expected, actual);
}

TEST(IPV6EndPointTest, ParseInvalid)
{
    const std::string text1 = "[ffffe:fdfc:fbfa:f9f8:f7f6:f5f4:f3f2:f1f0]:12345";
    const std::string text2 = "fffe:fdfc:fbfa:f9f8:f7f6:f5f4:f3f2:f1f0:0";
    const std::string text3 = "[fffe:fdfc:fbfa:f9f8:f7f6:f5f4:f3f2:f1f0]:123456";

    EXPECT_THROW(IPV6EndPoint::Parse(text1), std::runtime_error);
    EXPECT_THROW(IPV6EndPoint::Parse(text2), std::runtime_error);
    EXPECT_THROW(IPV6EndPoint::Parse(text3), std::runtime_error);
}

TEST(IPV6EndPointTest, TryParseIPAddressOnly)
{
    const std::string text = "fffe:fdfc:fbfa:f9f8:f7f6:f5f4:f3f2:f1f0";
    IPV6Address ipAddress({ 255, 254, 253, 252, 251, 250, 249, 248, 247, 246, 245, 244, 243, 242, 241, 240 });
    IPV6EndPoint expected(ipAddress);
    IPV6EndPoint actual;
    EXPECT_TRUE(IPV6EndPoint::TryParse(text, actual));
    EXPECT_EQ(expected, actual);
}

TEST(IPV6EndPointTest, TryParse)
{
    const std::string text = "[fffe:fdfc:fbfa:f9f8:f7f6:f5f4:f3f2:f1f0]:8080";
    IPV6Address ipAddress({ 255, 254, 253, 252, 251, 250, 249, 248, 247, 246, 245, 244, 243, 242, 241, 240 });
    uint16_t port = 8080;
    IPV6EndPoint actual;
    IPV6EndPoint expected(ipAddress, port);
    EXPECT_TRUE(IPV6EndPoint::TryParse(text, actual));
    EXPECT_EQ(expected, actual);
}

TEST(IPV6EndPointTest, TryParseInvalid)
{
    const std::string text1 = "[ffffe:fdfc:fbfa:f9f8:f7f6:f5f4:f3f2:f1f0]:12345";
    const std::string text2 = "fffe:fdfc:fbfa:f9f8:f7f6:f5f4:f3f2:f1f0:0";
    const std::string text3 = "[fffe:fdfc:fbfa:f9f8:f7f6:f5f4:f3f2:f1f0]:123456";
    IPV6EndPoint ipAddress;
    EXPECT_FALSE(IPV6EndPoint::TryParse(text1, ipAddress));
    EXPECT_FALSE(IPV6EndPoint::TryParse(text2, ipAddress));
    EXPECT_FALSE(IPV6EndPoint::TryParse(text3, ipAddress));
}

TEST(IPV6EndPointTest, OperatorEqual)
{
    IPV6Address ipAddress(IPV6Address::AddressType { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 });
    uint16_t port = 1234;
    IPV6EndPoint target(ipAddress, port);
    IPV6EndPoint ref1;
    IPV6EndPoint ref2(ipAddress);
    IPV6EndPoint ref3(ipAddress, port);
    IPV6EndPoint ref4(port);
    EXPECT_FALSE(target == ref1);
    EXPECT_FALSE(target == ref2);
    EXPECT_TRUE(target == ref3);
    EXPECT_FALSE(target == ref4);
    EXPECT_FALSE(ref1 == target);
    EXPECT_FALSE(ref2 == target);
    EXPECT_TRUE(ref3 == target);
    EXPECT_FALSE(ref4 == target);
}

TEST(IPV6EndPointTest, OperatorNotEqual)
{
    IPV6Address ipAddress(IPV6Address::AddressType { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 });
    uint16_t port = 1234;
    IPV6EndPoint target(ipAddress, port);
    IPV6EndPoint ref1;
    IPV6EndPoint ref2(ipAddress);
    IPV6EndPoint ref3(ipAddress, port);
    IPV6EndPoint ref4(port);
    EXPECT_TRUE(target != ref1);
    EXPECT_TRUE(target != ref2);
    EXPECT_FALSE(target != ref3);
    EXPECT_TRUE(target != ref4);
    EXPECT_TRUE(ref1 != target);
    EXPECT_TRUE(ref2 != target);
    EXPECT_FALSE(ref3 != target);
    EXPECT_TRUE(ref4 != target);
}

TEST(IPV6EndPointTest, StreamInsertionNoPortNoScopeID)
{
    IPV6Address ipAddress(IPV6Address::AddressType { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 });
    IPV6EndPoint endPoint(ipAddress, 0);
    const std::string expected = "102:304:506:708:90a:b0c:d0e:f10";
    std::ostringstream stream;
    stream << ipAddress;
    EXPECT_EQ(expected, stream.str());
}

TEST(IPV6EndPointTest, StreamInsertionNoPort)
{
    IPV6Address ipAddress(IPV6Address::AddressType { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 });
    IPV6EndPoint endPoint(ipAddress, 0, 0, 2);
    const std::string expected = "102:304:506:708:90a:b0c:d0e:f10%2";
    std::ostringstream stream;
    stream << endPoint;
    EXPECT_EQ(expected, stream.str());
}

TEST(IPV6EndPointTest, StreamInsertionNoScopeID)
{
    IPV6Address ipAddress(IPV6Address::AddressType { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 });
    IPV6EndPoint endPoint(ipAddress, 1234);
    const std::string expected = "[102:304:506:708:90a:b0c:d0e:f10]:1234";
    std::ostringstream stream;
    stream << endPoint;
    EXPECT_EQ(expected, stream.str());
}

TEST(IPV6EndPointTest, StreamExtraction)
{
    const std::string text = "[102:304:506:708:90a:b0c:d0e:f10]:1234%2";
    IPV6Address ipAddress({ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 });
    IPV6EndPoint expected(ipAddress, 1234, 0, 2);
    IPV6EndPoint actual;
    std::istringstream stream(text);
    stream >> actual;
    EXPECT_FALSE(stream.bad());
    EXPECT_EQ(expected, actual);
}

TEST(IPV6EndPointTest, StreamInsertion)
{
    IPV6Address ipAddress({ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 });
    IPV6EndPoint endPoint(ipAddress, 1234, 0, 2);
    const std::string expected = "[102:304:506:708:90a:b0c:d0e:f10]:1234%2";
    std::ostringstream stream;
    stream << endPoint;
    EXPECT_EQ(expected, stream.str());
}

TEST(IPV6EndPointTest, Deserialize)
{
    IPV6Address ipAddress({ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 });
    IPV6EndPoint expected(ipAddress, 1234, 0, 2);
    IPV6EndPoint actual;
    EXPECT_TRUE(serialization::Deserialize("[102:304:506:708:90a:b0c:d0e:f10]:1234%2", actual));
    EXPECT_EQ(expected, actual);
}

TEST(IPV6EndPointTest, Serialize)
{
    const std::string expected = "[102:304:506:708:90a:b0c:d0e:f10]:1234%2";
    IPV6Address ipAddress({ 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 });
    IPV6EndPoint endPoint(ipAddress, 1234, 0, 2);
    auto actual = serialization::Serialize(endPoint, 0);
    EXPECT_EQ(expected, actual);
}

} // namespace network
