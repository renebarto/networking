#include "network-osal/IPV6Address.h"

#include <iomanip>
#include "utility/Assert.h"
#include "utility/GenericError.h"
#include "tracing/Tracing.h"
#include "network-osal/SocketDefinitions.h"

namespace network {

IPV6Address const IPV6Address::None = IPV6Address({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0});
IPV6Address const IPV6Address::Any = IPV6Address({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0});
IPV6Address const IPV6Address::Broadcast = IPV6Address({255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255});
IPV6Address const IPV6Address::LocalHost = IPV6Address({0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1});

IPV6Address::IPV6Address()
    : m_address()
{}

IPV6Address::IPV6Address(const IPV6Address & other)
    : m_address(other.m_address)
{}

IPV6Address::IPV6Address(const std::initializer_list<std::uint8_t> & ipAddress)
    : m_address()
{
    ASSERT(ipAddress.size() == AddressSize);
    std::copy(ipAddress.begin(), ipAddress.end(), m_address.begin());
}
IPV6Address::IPV6Address(const AddressType & address)
    : m_address(address)
{}

IPV6Address::IPV6Address(const struct in6_addr & address)
    : m_address()
{
    std::copy(std::begin(address.s6_addr), std::end(address.s6_addr), std::begin(m_address));
}

IPV6Address::~IPV6Address()
{}

IPV6Address & IPV6Address::operator = (const IPV6Address & other)
{
    if (this != &other)
    {
        m_address = other.m_address;
    }
    return *this;
}

IPV6Address IPV6Address::Parse(const std::string & text)
{
    IPV6Address ipAddress;
    if (!TryParse(text, ipAddress))
    {
        tracing::Tracing::Throw(__FILE__, __LINE__, __func__, 
            utility::GenericError("IPV6Address string representation must be formatted as "
                                  "xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx (or a shortened format), string is {}", text));
    }
    return ipAddress;
}

bool IPV6Address::TryParse(const std::string & text, IPV6Address & ipAddress)
{
    in6_addr address;
    int result = inet_pton(AF_INET6, text.c_str(), &address);
    if (result == 0)
    {
        // Weird, when specifying "localhost", the hostname does not (always) resolve
        // So create a hack to return the local address in this case.
        if (utility::IsEqualIgnoreCase(text, "localhost"))
        {
            AddressType localAddress { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1 };
            std::copy(std::begin(localAddress), std::end(localAddress), std::begin(address.s6_addr));
        }
        else
        {
            addrinfo * addressInfo;
            addrinfo hints = { 0, AF_INET6, 0, 0, 0, nullptr, nullptr, nullptr };
            int errorCode = getaddrinfo(text.c_str(), nullptr, &hints, &addressInfo);
            if (errorCode != 0)
                return false;
            address = reinterpret_cast<sockaddr_in6 *>(addressInfo->ai_addr)->sin6_addr;
            freeaddrinfo(addressInfo);
        }
    }
    ipAddress = IPV6Address(address);
    return true;
}

in6_addr IPV6Address::ConvertAddress() const
{
    in6_addr result {};
    std::copy(std::begin(m_address), std::end(m_address), std::begin(result.s6_addr));
    return result;
}

bool operator == (const IPV6Address & lhs, const IPV6Address & rhs)
{
    return (lhs.m_address == rhs.m_address);
}
bool operator == (const IPV6Address::AddressType & lhs, const IPV6Address & rhs)
{
    return (lhs == rhs.m_address);
}
bool operator == (const IPV6Address & lhs, const IPV6Address::AddressType & rhs)
{
    return (lhs.m_address == rhs);
}
bool operator != (const IPV6Address & lhs, const IPV6Address & rhs)
{
    return !operator == (lhs, rhs);
}
bool operator != (const IPV6Address::AddressType & lhs, const IPV6Address & rhs)
{
    return !operator == (lhs, rhs);
}
bool operator != (const IPV6Address & lhs, const IPV6Address::AddressType & rhs)
{
    return !operator == (lhs, rhs);
}

std::istream & operator >> (std::istream & stream, IPV6Address & value)
{
    std::string text;
    stream >> text;
    if (!stream.fail())
    {
        if (!IPV6Address::TryParse(text, value))
            stream.clear(stream.failbit);
    }
    return stream;
}

std::ostream & operator << (std::ostream & stream, const IPV6Address & value)
{
    constexpr size_t NumWords = IPV6Address::AddressSize / 2;
    std::uint16_t words[NumWords];
    size_t zeroSequenceStartMax = 0;
    size_t zeroSequenceLengthMax = 0;
    size_t zeroSequenceStart = 0;
    size_t zeroSequenceLength = 0;
    bool inZeroSequence = false;
    size_t offset = 0;
    for (size_t i = 0; i < NumWords; ++i)
    {
        words[i] = static_cast<std::uint16_t>(value.m_address[offset] << 8 | value.m_address[offset + 1]);
        offset += 2;
        if (words[i] == 0)
        {
            if (!inZeroSequence)
            {
                zeroSequenceStart = i;
                inZeroSequence = true;
            }
        }
        else
        {
            if (inZeroSequence)
            {
                zeroSequenceLength = i - zeroSequenceStart;
                inZeroSequence = false;
                if (zeroSequenceLength > zeroSequenceLengthMax)
                {
                    zeroSequenceStartMax = zeroSequenceStart;
                    zeroSequenceLengthMax = zeroSequenceLength;
                }
                zeroSequenceStart = static_cast<size_t>(-1);
                zeroSequenceLength = 0;
            }
        }
    }
    if (inZeroSequence)
    {
        zeroSequenceLength = NumWords - zeroSequenceStart;
        if (zeroSequenceLength > zeroSequenceLengthMax)
        {
            zeroSequenceStartMax = zeroSequenceStart;
            zeroSequenceLengthMax = zeroSequenceLength;
        }

    }
    stream << std::hex;
    for (size_t i = 0; i < NumWords; ++i)
    {
        if ((i == zeroSequenceStartMax) && (zeroSequenceLengthMax > 1))
        {
            stream << "::";
        }
        else if ((i == zeroSequenceStartMax) && (zeroSequenceLengthMax == 1))
        {
            stream << ":0:";
        }
        else if ((i < zeroSequenceStartMax) || (i >= zeroSequenceStartMax + zeroSequenceLengthMax))
        {
            stream << words[i];
            if (((i + 1) < NumWords) && ((i + 1) != zeroSequenceStartMax))
                stream << ":";
        }
    }
    return stream << std::dec;
}

} // namespace network