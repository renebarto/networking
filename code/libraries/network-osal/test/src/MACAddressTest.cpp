#include "GoogleTest.h"

#include "network-osal/MACAddress.h"
#include "utility/Deserialization.h"
#include "utility/Serialization.h"

namespace network {

TEST(MACAddress, Construct)
{
    MACAddress address;

    EXPECT_EQ(MACAddress::AddressType({0, 0, 0, 0}), address.Address());
    EXPECT_EQ("00-00-00-00-00-00", serialization::Serialize(address, 0));
}

TEST(MACAddress, ConstructCopy)
{
    MACAddress other({1, 2, 3, 4, 5, 6});
    MACAddress address(other);

    EXPECT_EQ(MACAddress::AddressType({1, 2, 3, 4, 5, 6}), address.Address());
    EXPECT_EQ("01-02-03-04-05-06", serialization::Serialize(address, 0));
}

TEST(MACAddress, ConstructFromAddressType)
{
    MACAddress address(MACAddress::AddressType {1, 2, 3, 4, 5, 6});

    EXPECT_EQ(MACAddress::AddressType({1, 2, 3, 4, 5, 6}), address.Address());
    EXPECT_EQ("01-02-03-04-05-06", serialization::Serialize(address, 0));
}

TEST(MACAddress, ConstructFromInitializer)
{
    MACAddress address({1, 2, 3, 4, 5, 6});

    EXPECT_EQ(MACAddress::AddressType({1, 2, 3, 4, 5, 6}), address.Address());
    EXPECT_EQ("01-02-03-04-05-06", serialization::Serialize(address, 0));
}

TEST(MACAddress, Assign)
{
    MACAddress other({1, 2, 3, 4, 5, 6});
    MACAddress address;
    address = other;

    EXPECT_EQ(MACAddress::AddressType({1, 2, 3, 4, 5, 6}), address.Address());
    EXPECT_EQ("01-02-03-04-05-06", serialization::Serialize(address, 0));
}

TEST(MACAddressTest, Parse)
{
    const std::string text = "FF-FE-FD-01-02-03";
    MACAddress::AddressType macAddress {255, 254, 253, 1, 2, 3};
    MACAddress expected(macAddress);
    const MACAddress & actual = MACAddress::Parse(text);
    EXPECT_EQ(expected, actual);
}

TEST(MACAddressTest, ParseInvalid)
{
    const std::string text1 = "00-11-22-AA-BB";
    const std::string text2 = "XX-YY-ZZ-AA-BB-CC";

    EXPECT_THROW(MACAddress::Parse(text1), std::runtime_error);
    EXPECT_THROW(MACAddress::Parse(text2), std::runtime_error);
}

TEST(MACAddressTest, TryParse)
{
    const std::string text = "FF-FE-FD-01-02-03";
    MACAddress actual;
    MACAddress expected({255, 254, 253, 1, 2, 3});
    EXPECT_TRUE(MACAddress::TryParse(text, actual));
    EXPECT_EQ(expected, actual);
}

TEST(MACAddressTest, TryParseInvalid)
{
    const std::string text1 = "00-11-22-AA-BB";
    const std::string text2 = "XX-YY-ZZ-AA-BB-CC";
    MACAddress macAddress;
    EXPECT_FALSE(MACAddress::TryParse(text1, macAddress));
    EXPECT_FALSE(MACAddress::TryParse(text2, macAddress));
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
    MACAddress target(macAddress);
    MACAddress ref1;
    MACAddress ref2({0, 0, 0, 0, 0, 0});
    MACAddress ref3(macAddress);
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

TEST(MACAddressTest, OperatorNotEqual)
{
    MACAddress::AddressType macAddress {1, 2, 3, 4, 5, 6};
    MACAddress target(macAddress);
    MACAddress ref1;
    MACAddress ref2({0, 0, 0, 0, 0, 0});
    MACAddress ref3(macAddress);
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

TEST(MACAddressTest, StreamExtraction)
{
    const std::string text = "01-02-03-04-05-06";
    MACAddress expected({1, 2, 3, 4, 5, 6});
    MACAddress actual;
    std::istringstream stream(text);
    stream >> actual;
    EXPECT_FALSE(stream.bad());
    EXPECT_EQ(expected, actual);
}

TEST(MACAddressTest, StreamInsertion)
{
    MACAddress target({1, 2, 3, 4, 5, 6});
    const std::string expected = "01-02-03-04-05-06";
    std::ostringstream stream;
    stream << target;
    EXPECT_EQ(expected, stream.str());
}

TEST(MACAddressTest, Deserialize)
{
    MACAddress expected({1, 2, 3, 4, 5, 6});
    MACAddress actual;
    EXPECT_TRUE(serialization::Deserialize("01-02-03-04-05-06", actual));
    EXPECT_EQ(expected, actual);
}

TEST(MACAddressTest, Serialize)
{
    MACAddress target({1, 2, 3, 4, 5, 6});
    const std::string expected = "01-02-03-04-05-06";
    EXPECT_EQ(expected, serialization::Serialize(target, 0));
}

} // namespace network
