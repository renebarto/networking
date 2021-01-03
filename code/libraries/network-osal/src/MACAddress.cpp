#include "network-osal/MACAddress.h"

#include <algorithm>
#include <iomanip>
#include "utility/Assert.h"
#include "utility/GenericError.h"
#include "tracing/Tracing.h"

namespace network {

MACAddress::MACAddress()
    : m_address()
{}

MACAddress::MACAddress(const MACAddress & other)
    : m_address(other.m_address)
{
}
MACAddress::MACAddress(const AddressType & address)
    : m_address(address)
{}

MACAddress::MACAddress(const std::uint8_t address[], std::uint8_t size)
    : m_address()
{
    ASSERT(size == AddressSize);
    std::copy_n(address, size, std::begin(m_address));
}

MACAddress::MACAddress(const std::initializer_list<std::uint8_t> & address)
    : m_address()
{
    ASSERT(address.size() == AddressSize);
    std::copy(std::begin(address), std::end(address), std::begin(m_address));
}

MACAddress::~MACAddress()
{}

MACAddress & MACAddress::operator = (const MACAddress & other)
{
    if (this != &other)
    {
        m_address = other.m_address;
    }
    return *this;
}

MACAddress MACAddress::Parse(const std::string & text)
{
    MACAddress macAddress;
    if (!TryParse(text, macAddress))
    {
        tracing::Tracer::Throw(__FILE__, __LINE__, __func__, 
            utility::GenericError("MACAddress string representation must be formatted as xx-xx-xx-xx-xx-xx, string is {}", text));
    }
    return macAddress;
}

static bool ParseByte(const std::string & text, uint8_t & value)
{
    if (text.length() != 2)
        return false;
    return serialization::Deserialize(text, value, 16);
}

bool MACAddress::TryParse(const std::string & text, MACAddress & macAddress)
{
    std::string str = text;
    const char delimiter = '-';
    MACAddress result;

    size_t pos = 0;
    std::string token;
    uint8_t value;
    size_t index = 0;
    while (((pos = str.find(delimiter)) != std::string::npos) && (index < AddressSize))
    {
        token = str.substr(0, pos);
        if (!ParseByte(token, value))
            return false;
        result.m_address[index++] = value;
        str.erase(0, pos + 1);
    }
    if ((index >= AddressSize))
        return false;
    if (!ParseByte(str, value))
        return false;
    result.m_address[index++] = value;
    if (index == AddressSize)
    {
        macAddress = result;
        return true;
    }
    return false;
}

bool operator == (const MACAddress & lhs, const MACAddress & rhs)
{
    return (lhs.m_address == rhs.m_address);
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

std::istream & operator >> (std::istream & stream, MACAddress & value)
{
    std::string text;
    stream >> text;
    if (!stream.fail())
    {
        if (!MACAddress::TryParse(text, value))
            stream.clear(stream.failbit);
    }
    return stream;
}

std::ostream & operator << (std::ostream & stream, const MACAddress & value)
{
    return stream << std::hex << std::setfill('0')
        << std::uppercase << std::setw(2) << static_cast<int>(value.Address()[size_t{0}]) << "-"
        << std::uppercase << std::setw(2) << static_cast<int>(value.Address()[size_t{1}]) << "-"
        << std::uppercase << std::setw(2) << static_cast<int>(value.Address()[size_t{2}]) << "-"
        << std::uppercase << std::setw(2) << static_cast<int>(value.Address()[size_t{3}]) << "-"
        << std::uppercase << std::setw(2) << static_cast<int>(value.Address()[size_t{4}]) << "-"
        << std::uppercase << std::setw(2) << static_cast<int>(value.Address()[size_t{5}]) << std::dec;
}

} // namespace network