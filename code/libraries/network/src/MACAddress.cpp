#include "network/MACAddress.h"

#include <iomanip>

namespace network {

std::ostream & operator << (std::ostream & stream, const MACAddress & value)
{
    return stream << std::hex << std::setfill('0')
        << std::uppercase << std::setw(2) << static_cast<int>(value.m_address[size_t{0}]) << "-"
        << std::uppercase << std::setw(2) << static_cast<int>(value.m_address[size_t{1}]) << "-"
        << std::uppercase << std::setw(2) << static_cast<int>(value.m_address[size_t{2}]) << "-"
        << std::uppercase << std::setw(2) << static_cast<int>(value.m_address[size_t{3}]) << "-"
        << std::uppercase << std::setw(2) << static_cast<int>(value.m_address[size_t{4}]) << "-"
        << std::uppercase << std::setw(2) << static_cast<int>(value.m_address[size_t{5}]);
}

bool operator == (const MACAddress & lhs, const MACAddress & rhs)
{
    return (lhs.m_address == rhs.m_address) && (lhs.m_interfaceIndex == rhs.m_interfaceIndex);
}
bool operator == (const MACAddress::AddressType & lhs, const MACAddress & rhs)
{
    return (lhs == rhs.m_address);
}
bool operator == (const MACAddress & lhs, const MACAddress::AddressType & rhs)
{
    return (lhs.m_address == rhs);
}
bool operator != (const MACAddress & lhs, const MACAddress & rhs)
{
    return !operator == (lhs, rhs);
}
bool operator != (const MACAddress::AddressType & lhs, const MACAddress & rhs)
{
    return !operator == (lhs, rhs);
}
bool operator != (const MACAddress & lhs, const MACAddress::AddressType & rhs)
{
    return !operator == (lhs, rhs);
}

} // namespace network