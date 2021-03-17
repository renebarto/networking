#include "network-osal/IPV4Address.h"

#include "osal/Utilities.h"
#include "utility/Assert.h"
#include "utility/GenericError.h"
#include "tracing/Tracing.h"
#include "network-osal/SocketDefinitions.h"

namespace network {

IPV4Address const IPV4Address::None = IPV4Address({0, 0, 0, 0});
IPV4Address const IPV4Address::Any = IPV4Address({0, 0, 0, 0});
IPV4Address const IPV4Address::Broadcast = IPV4Address({255, 255, 255, 255});
IPV4Address const IPV4Address::LocalHost = IPV4Address({127, 0, 0, 1});

IPV4Address::IPV4Address()
    : m_address()
{}

IPV4Address::IPV4Address(const IPV4Address & other)
    : m_address(other.m_address)
{}

IPV4Address::IPV4Address(const std::initializer_list<std::uint8_t> & ipAddress)
    : m_address()
{
    ASSERT(ipAddress.size() == AddressSize);
    std::copy(ipAddress.begin(), ipAddress.end(), m_address.begin());
}

IPV4Address::IPV4Address(const AddressType & address)
    : m_address(address)
{}

IPV4Address::IPV4Address(const struct in_addr & address)
    : m_address()
{
    // Always platform byte order
    SetUInt32(address.s_addr);
}

IPV4Address::~IPV4Address()
{}

IPV4Address & IPV4Address::operator = (const IPV4Address & other)
{
    if (this != &other)
    {
        m_address = other.m_address;
    }
    return *this;
}

IPV4Address IPV4Address::Parse(const std::string & text)
{
    IPV4Address ipAddress;
    if (!TryParse(text, ipAddress))
    {
        tracing::Tracing::Throw(__FILE__, __LINE__, __func__, 
            utility::GenericError("IPV4Address string representation must be formatted as ddd.ddd.ddd.ddd"
                                  ", string is {}", text));
    }
    return ipAddress;
}

bool IPV4Address::TryParse(const std::string & text, IPV4Address & ipAddress)
{
    in_addr address;
    int result = inet_pton(AF_INET, text.c_str(), &address);
    if (result == 0)
    {
        addrinfo * addressInfo;
        addrinfo hints = { 0, AF_INET, 0, 0, 0, nullptr, nullptr, nullptr };
        int errorCode = getaddrinfo(text.c_str(), nullptr, &hints, &addressInfo);
        if (errorCode != 0)
            return false;
        address = ((sockaddr_in *)(addressInfo->ai_addr))->sin_addr;
        freeaddrinfo(addressInfo);
    }
    ipAddress = IPV4Address(address);
    return true;
}

std::uint32_t IPV4Address::GetUInt32() const
{
    std::uint32_t address;
    std::copy(m_address.begin(), m_address.end(), reinterpret_cast<std::uint8_t *>(&address));
    return address;
}

void IPV4Address::SetUInt32(std::uint32_t value)
{
    std::copy(reinterpret_cast<const std::uint8_t *>(&value), reinterpret_cast<const std::uint8_t *>(&value + 1), std::begin(m_address));
}

in_addr IPV4Address::ConvertAddress() const 
{
    in_addr result {};
    result.s_addr = GetUInt32();
    return result;
}

bool operator == (const IPV4Address & lhs, const IPV4Address & rhs)
{
    return (lhs.m_address == rhs.m_address);
}
bool operator == (std::uint32_t lhs, const IPV4Address & rhs)
{
    return (lhs == rhs.GetUInt32());
}
bool operator == (const IPV4Address & lhs, std::uint32_t rhs)
{
    return (lhs.GetUInt32() == rhs);
}
bool operator == (const IPV4Address::AddressType & lhs, const IPV4Address & rhs)
{
    return (lhs == rhs.m_address);
}
bool operator == (const IPV4Address & lhs, const IPV4Address::AddressType & rhs)
{
    return (lhs.m_address == rhs);
}
bool operator != (const IPV4Address & lhs, const IPV4Address & rhs)
{
    return !operator == (lhs, rhs);
}
bool operator != (std::uint32_t lhs, const IPV4Address & rhs)
{
    return !operator == (lhs, rhs);
}
bool operator != (const IPV4Address & lhs, std::uint32_t rhs)
{
    return !operator == (lhs, rhs);
}
bool operator != (const IPV4Address::AddressType & lhs, const IPV4Address & rhs)
{
    return !operator == (lhs, rhs);
}
bool operator != (const IPV4Address & lhs, const IPV4Address::AddressType & rhs)
{
    return !operator == (lhs, rhs);
}

std::istream & operator >> (std::istream & stream, IPV4Address & value)
{
    std::string text;
    stream >> text;
    if (!stream.fail())
    {
        if (!IPV4Address::TryParse(text, value))
            stream.clear(stream.failbit);
    }
    return stream;
}

std::ostream & operator << (std::ostream & stream, const IPV4Address & value)
{
    return stream 
        << static_cast<int>(value.m_address[size_t{0}]) << "." 
        << static_cast<int>(value.m_address[size_t{1}]) << "."
        << static_cast<int>(value.m_address[size_t{2}]) << "."
        << static_cast<int>(value.m_address[size_t{3}]);
}

} // namespace network