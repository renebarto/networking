#include "network/IPV4Address.h"

namespace network {

IPV4Address const IPV4Address::None = IPV4Address({0, 0, 0, 0});
IPV4Address const IPV4Address::Any = IPV4Address({0, 0, 0, 0});
IPV4Address const IPV4Address::Broadcast = IPV4Address({255, 255, 255, 255});
IPV4Address const IPV4Address::LocalHost = IPV4Address({127, 0, 0, 1});

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

std::ostream & operator << (std::ostream & stream, const IPV4Address & value)
{
    return stream 
        << static_cast<int>(value.m_address[size_t{0}]) << "." 
        << static_cast<int>(value.m_address[size_t{1}]) << "."
        << static_cast<int>(value.m_address[size_t{2}]) << "."
        << static_cast<int>(value.m_address[size_t{3}]);
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

} // namespace network