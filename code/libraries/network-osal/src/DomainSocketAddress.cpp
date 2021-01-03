#include "network-osal/DomainSocketAddress.h"

#include <netdb.h>
#include <arpa/inet.h>
#include <sys/un.h>
#include <sstream>
#include "utility/Assert.h"
#include "utility/GenericError.h"
#include "tracing/Tracing.h"

namespace network {

const DomainSocketAddress DomainSocketAddress::None = DomainSocketAddress("");
const DomainSocketAddress DomainSocketAddress::Any = DomainSocketAddress("");
const DomainSocketAddress DomainSocketAddress::Broadcast = DomainSocketAddress("");
const DomainSocketAddress DomainSocketAddress::LocalHost = DomainSocketAddress("");

DomainSocketAddress::DomainSocketAddress()
    : m_address(None.m_address)
{
}
DomainSocketAddress::DomainSocketAddress(const DomainSocketAddress & other)
    : m_address(other.m_address)
{
}
DomainSocketAddress::DomainSocketAddress(const AddressType & address)
    : m_address()
{
    std::copy(address.begin(), address.end(), m_address.begin());
}
DomainSocketAddress::DomainSocketAddress(const std::initializer_list<std::uint8_t> & address)
    : m_address()
{
    ASSERT(address.size() <= AddressSize);
    std::copy(address.begin(), address.end(), m_address.begin());
}
DomainSocketAddress::DomainSocketAddress(const std::string & address)
    : m_address()
{
    ASSERT(address.length() <= AddressSize);
    std::copy(address.begin(), address.end(), m_address.begin());
}

DomainSocketAddress::~DomainSocketAddress()
{
}

DomainSocketAddress & DomainSocketAddress::operator = (const DomainSocketAddress & other)
{
    if (this != &other)
    {
        m_address = other.m_address;
    }
    return *this;
}

DomainSocketAddress DomainSocketAddress::Parse(const std::string & text)
{
    DomainSocketAddress address;
    if (!TryParse(text, address))
    {
        tracing::Tracer::Throw(__FILE__, __LINE__, __func__, 
            utility::GenericError("DomainSocketAddress string representation must be a UNIX path of no more than "
                                  "{} bytes, string is {}", AddressSize, text));
    }
    return address;
}

bool DomainSocketAddress::TryParse(const std::string & text, DomainSocketAddress & address)
{
    if (text.length() > AddressSize)
    {
        address = DomainSocketAddress();
        return false;
    }
    address = DomainSocketAddress(text);
    return true;
}

bool operator == (const DomainSocketAddress & lhs, const DomainSocketAddress & rhs)
{
    return (lhs.m_address == rhs.m_address);
}
bool operator == (const DomainSocketAddress::AddressType & lhs, const DomainSocketAddress & rhs)
{
    return (lhs == rhs.m_address);
}
bool operator == (const DomainSocketAddress & lhs, const DomainSocketAddress::AddressType & rhs)
{
    return (lhs.m_address == rhs);
}
bool operator != (const DomainSocketAddress & lhs, const DomainSocketAddress & rhs)
{
    return !operator == (lhs, rhs);
}
bool operator != (const DomainSocketAddress::AddressType & lhs, const DomainSocketAddress & rhs)
{
    return !operator == (lhs, rhs);
}
bool operator != (const DomainSocketAddress & lhs, const DomainSocketAddress::AddressType & rhs)
{
    return !operator == (lhs, rhs);
}

std::istream & operator >> (std::istream & stream, DomainSocketAddress & value)
{
    std::string text;
    stream >> text;
    if (!stream.fail())
    {
        if (!DomainSocketAddress::TryParse(text, value))
            stream.clear(stream.failbit);
    }
    return stream;
}

std::ostream & operator << (std::ostream & stream, const DomainSocketAddress & value)
{
    return stream << reinterpret_cast<const char *>(value.Address().data());
}

} // namespace network