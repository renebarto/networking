#include "GoogleTest.h"

#include "network-osal/DomainSocketAddress.h"
#include "utility/Assert.h"
#include "utility/Serialization.h"

namespace network {

class DomainSocketAddressTest
    : public ::testing::Test
{
public:
    static void AssertionHandler(bool expression, const char * expressionText, const char * file, int line, const char * func)
    {
        _hadAssertion = true;
        _expression = expression;
        _expressionText = expressionText;
        _file = file;
        _line = line;
        _func = func;
    }
    static bool _hadAssertion;
    static bool _expression;
    static const char * _expressionText;
    static const char * _file;
    static int _line;
    static const char * _func;

    void SetUp() override
    {
        _hadAssertion = false;
        _expression = false;
        _expressionText = nullptr;
        _file = nullptr;
        _line = 0;
        _func = nullptr;
        utility::SetAssertionHandler(AssertionHandler);
    }
};

bool DomainSocketAddressTest::_hadAssertion = false;
bool DomainSocketAddressTest::_expression = false;
const char * DomainSocketAddressTest::_expressionText = nullptr;
const char * DomainSocketAddressTest::_file = nullptr;
int DomainSocketAddressTest::_line = 0;
const char * DomainSocketAddressTest::_func = nullptr;

TEST_F(DomainSocketAddressTest, Constructor)
{
    DomainSocketAddress target;
    DomainSocketAddress::AddressType reference { 0 };
    const std::string expected = "";
    EXPECT_TRUE(reference == target.Address());
    EXPECT_EQ(expected, serialization::Serialize(target, 0));
}

// TEST_F(DomainSocketAddressTest, ConstructorByteArray)
// {
//     DomainSocketAddress::AddressType domainAddress({0x31, 0x32, 0x33, 0x34, 0x35, 0x36});
//     DomainSocketAddress target(domainAddress);
//     const std::string expected = "123456";
//     DomainSocketAddress::AddressType expectedAddress;
//     target.GetBytes().get(0, expectedAddress, domainAddress.size());
//     EXPECT_EQ(domainAddress, expectedAddress);
//     ostringstream stream;
//     target.PrintTo(stream);
//     EXPECT_EQ(expected, stream.str());
// }

// TEST_F(DomainSocketAddressTest, ConstructorByteArrayOffset)
// {
//     DomainSocketAddress::AddressType reference({0x31, 0x32, 0x33, 0x34, 0x35, 0x36});
//     DomainSocketAddress target({0x30, 0x31, 0x32, 0x33, 0x34, 0x35, 0x36}, 1);
//     const std::string expected = "123456";
//     DomainSocketAddress::AddressType expectedAddress;
//     target.GetBytes().get(0, expectedAddress, reference.size());
//     EXPECT_EQ(reference, expectedAddress);
//     ostringstream stream;
//     target.PrintTo(stream);
//     EXPECT_EQ(expected, stream.str());
// }

// TEST_F(DomainSocketAddressTest, ConstructorByteArrayOffsetTooLarge)
// {
//     DomainSocketAddress::AddressType reference(DomainSocketAddress::AddressSize);
//     DomainSocketAddress::AddressType data(DomainSocketAddress::AddressSize + 1);

//     EXPECT_FALSE(_hadAssertion);
//     DomainSocketAddress target(data, 0);
//     EXPECT_TRUE(_hadAssertion);

//     DomainSocketAddress::AddressType expectedAddress;
//     target.GetBytes().get(0, expectedAddress, reference.size());
//     EXPECT_EQ(reference, expectedAddress);
// }

// TEST_F(DomainSocketAddressTest, ConstructorCopy)
// {
//     DomainSocketAddress::AddressType domainAddress({0x31, 0x32, 0x33, 0x34, 0x35, 0x36});
//     DomainSocketAddress other(domainAddress);
//     DomainSocketAddress target(other);
//     const std::string expected = "123456";
//     DomainSocketAddress::AddressType expectedAddress;
//     target.GetBytes().get(0, expectedAddress, domainAddress.size());
//     EXPECT_EQ(domainAddress, expectedAddress);
//     ostringstream stream;
//     target.PrintTo(stream);
//     EXPECT_EQ(expected, stream.str());
// }

// TEST_F(DomainSocketAddressTest, Assign)
// {
//     DomainSocketAddress::AddressType domainAddress({0x31, 0x32, 0x33, 0x34, 0x35, 0x36});
//     DomainSocketAddress other(domainAddress);
//     DomainSocketAddress target;
//     target = other;
//     const std::string expected = "123456";
//     DomainSocketAddress::AddressType expectedAddress;
//     target.GetBytes().get(0, expectedAddress, domainAddress.size());
//     EXPECT_EQ(domainAddress, expectedAddress);
//     ostringstream stream;
//     target.PrintTo(stream);
//     EXPECT_EQ(expected, stream.str());
// }

// TEST_F(DomainSocketAddressTest, CreateDefault)
// {
//     EndPointPtr target = DomainSocketAddress::Create();
//     ASSERT_NOT_NULL(target);
//     DomainSocketAddress::AddressType reference(DomainSocketAddress::AddressSize);
//     const std::string expected = "";
//     EXPECT_EQ(reference, target->GetBytes());
//     ostringstream stream;
//     target->PrintTo(stream);
//     EXPECT_EQ(expected, stream.str());
// }

// TEST_F(DomainSocketAddressTest, CreateFromEndPointDomainSocketAddress)
// {
//     DomainSocketAddress::AddressType domainAddress({0x31, 0x32, 0x33, 0x34, 0x35, 0x36});
//     DomainSocketAddress other(domainAddress);
//     EndPointPtr target = DomainSocketAddress::Create(other);
//     ASSERT_NOT_NULL(target);
//     const std::string expected = "123456";
//     DomainSocketAddress::AddressType expectedAddress;
//     target->GetBytes().get(0, expectedAddress, domainAddress.size());
//     EXPECT_EQ(domainAddress, expectedAddress);
//     ostringstream stream;
//     target->PrintTo(stream);
//     EXPECT_EQ(expected, stream.str());
// }

// TEST_F(DomainSocketAddressTest, CreateFromEndPointIPV4EndPoint)
// {
//     IPV4EndPoint other(12345);
//     EndPointPtr target = DomainSocketAddress::Create(other);
//     EXPECT_NULL(target);
// }

// TEST_F(DomainSocketAddressTest, CreateFromString)
// {
//     const std::string text = "/abc/def";
//     DomainSocketAddress::AddressType domainAddress(text);
//     DomainSocketAddress expected(domainAddress);
//     EndPointPtr actual = DomainSocketAddress::Create(text);
//     EXPECT_EQ(expected, *actual);
// }

// TEST_F(DomainSocketAddressTest, OperatorIndex)
// {
//     DomainSocketAddress::AddressType domainAddress({0x31, 0x32, 0x33, 0x34, 0x35, 0x36});
//     DomainSocketAddress target(domainAddress);
//     EXPECT_EQ(domainAddress[size_t{0}], target[0]);
//     EXPECT_EQ(domainAddress[size_t{1}], target[1]);
//     EXPECT_EQ(domainAddress[size_t{2}], target[2]);
//     EXPECT_EQ(domainAddress[size_t{3}], target[3]);
//     EXPECT_EQ(domainAddress[size_t{4}], target[4]);
//     EXPECT_EQ(domainAddress[size_t{5}], target[5]);
//     EXPECT_EQ(uint8_t {0}, target[6]);

//     EXPECT_FALSE(_hadAssertion);
//     EXPECT_EQ(target[DomainSocketAddress::AddressSize - 1],
//               target[DomainSocketAddress::AddressSize]);
//     EXPECT_TRUE(_hadAssertion);
// }

// TEST_F(DomainSocketAddressTest, Parse)
// {
//     const std::string text = "/abc/def";
//     DomainSocketAddress::AddressType domainAddress(text);
//     DomainSocketAddress expected(domainAddress);
//     const DomainSocketAddress & actual = DomainSocketAddress::Parse(text);
//     EXPECT_EQ(expected, actual);
// }

// TEST_F(DomainSocketAddressTest, ParseInvalid)
// {
//     const std::string textTooLong = "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678";

//     EXPECT_THROW(DomainSocketAddress::Parse(textTooLong), OSAL::ArgumentException);
// }

// TEST_F(DomainSocketAddressTest, TryParse)
// {
//     const std::string text = "/abc/def";
//     DomainSocketAddress::AddressType domainAddress(text);
//     DomainSocketAddress actual;
//     DomainSocketAddress expected(domainAddress);
//     EXPECT_TRUE(DomainSocketAddress::TryParse(text, actual));
//     EXPECT_EQ(expected, actual);
// }

// TEST_F(DomainSocketAddressTest, TryParseInvalid)
// {
//     const std::string textTooLong = "0123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678";

//     DomainSocketAddress domainAddress;
//     EXPECT_FALSE(DomainSocketAddress::TryParse(textTooLong, domainAddress));
// }

// TEST_F(DomainSocketAddressTest, OperatorEqualEndPoint)
// {
//     DomainSocketAddress::AddressType ipAddress({0x31, 0x32, 0x33, 0x34, 0x35, 0x36});
//     DomainSocketAddress target(ipAddress);
//     const EndPoint &ref1 = IPV4EndPoint::Parse("127.0.0.1");
//     DomainSocketAddress ref2(DomainSocketAddress::AddressType({0, 0, 0, 0, 0, 0}));
//     DomainSocketAddress ref3(ipAddress);
//     EXPECT_FALSE(target == ref1);
//     EXPECT_FALSE(target == ref2);
//     EXPECT_TRUE(target == ref3);
//     EXPECT_FALSE(ref2 == target);
//     EXPECT_FALSE(ref2 == ref1);
//     EXPECT_FALSE(ref2 == ref3);
//     EXPECT_TRUE(ref3 == target);
//     EXPECT_FALSE(ref3 == ref1);
//     EXPECT_FALSE(ref3 == ref2);
// }

// TEST_F(DomainSocketAddressTest, OperatorNotEqualEndPoint)
// {
//     DomainSocketAddress::AddressType ipAddress({0x31, 0x32, 0x33, 0x34, 0x35, 0x36});
//     DomainSocketAddress target(ipAddress);
//     const EndPoint &ref1 = IPV4EndPoint::Parse("127.0.0.1");
//     DomainSocketAddress ref2(DomainSocketAddress::AddressType({0, 0, 0, 0, 0, 0}));
//     DomainSocketAddress ref3(ipAddress);
//     EXPECT_TRUE(target != ref1);
//     EXPECT_TRUE(target != ref2);
//     EXPECT_FALSE(target != ref3);
//     EXPECT_TRUE(ref2 != target);
//     EXPECT_TRUE(ref2 != ref1);
//     EXPECT_TRUE(ref2 != ref3);
//     EXPECT_FALSE(ref3 != target);
//     EXPECT_TRUE(ref3 != ref1);
//     EXPECT_TRUE(ref3 != ref2);
// }

// TEST_F(DomainSocketAddressTest, OperatorEqual)
// {
//     DomainSocketAddress::AddressType domainAddress({0x31, 0x32, 0x33, 0x34, 0x35, 0x36});
//     DomainSocketAddress target(domainAddress);
//     DomainSocketAddress ref1;
//     DomainSocketAddress ref2(DomainSocketAddress::AddressType({0, 0, 0, 0, 0, 0}));
//     DomainSocketAddress ref3(domainAddress);
//     EXPECT_FALSE(target == ref1);
//     EXPECT_FALSE(target == ref2);
//     EXPECT_TRUE(target == ref3);
//     EXPECT_FALSE(ref1 == target);
//     EXPECT_TRUE(ref1 == ref2);
//     EXPECT_FALSE(ref1 == ref3);
//     EXPECT_FALSE(ref2 == target);
//     EXPECT_TRUE(ref2 == ref1);
//     EXPECT_FALSE(ref2 == ref3);
//     EXPECT_TRUE(ref3 == target);
//     EXPECT_FALSE(ref3 == ref1);
//     EXPECT_FALSE(ref3 == ref2);
// }

// TEST_F(DomainSocketAddressTest, OperatorNotEqual)
// {
//     DomainSocketAddress::AddressType domainAddress({0x31, 0x32, 0x33, 0x34, 0x35, 0x36});
//     DomainSocketAddress target(domainAddress);
//     DomainSocketAddress ref1;
//     DomainSocketAddress ref2(DomainSocketAddress::AddressType({0, 0, 0, 0, 0, 0}));
//     DomainSocketAddress ref3(domainAddress);
//     EXPECT_TRUE(target != ref1);
//     EXPECT_TRUE(target != ref2);
//     EXPECT_FALSE(target != ref3);
//     EXPECT_TRUE(ref1 != target);
//     EXPECT_FALSE(ref1 != ref2);
//     EXPECT_TRUE(ref1 != ref3);
//     EXPECT_TRUE(ref2 != target);
//     EXPECT_FALSE(ref2 != ref1);
//     EXPECT_TRUE(ref2 != ref3);
//     EXPECT_FALSE(ref3 != target);
//     EXPECT_TRUE(ref3 != ref1);
//     EXPECT_TRUE(ref3 != ref2);
// }

// TEST_F(DomainSocketAddressTest, PrintTo)
// {
//     const std::string path = "/tmp/socket";
//     DomainSocketAddress target(path);
//     const std::string expected = path;
//     ostringstream stream;
//     target.PrintTo(stream);
//     EXPECT_EQ(expected, stream.str());
// }

// TEST_F(DomainSocketAddressTest, PrintToNonMember)
// {
//     const std::string path = "/tmp/socket";
//     DomainSocketAddress target(path);
//     const std::string expected = path;
//     ostringstream stream;
//     PrintTo(stream, target);
//     EXPECT_EQ(expected, stream.str());
// }

} // namespace network
