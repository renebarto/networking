#include "network/IPV4EndPoint.h"

#include "utility/Endian.h"
#include "utility/GenericError.h"
#include "tracing/Tracing.h"

namespace network {

SockAddrIPV4::SockAddrIPV4()
    : sockaddr_in()
{
}

SockAddrIPV4::SockAddrIPV4(const SockAddrIPV4 & other)
    : sockaddr_in(other)
{
}

SockAddrIPV4::SockAddrIPV4(const sockaddr_in & other)
    : sockaddr_in(other)
{
}

SockAddrIPV4::SockAddrIPV4(const in_addr & address)
    : sockaddr_in()
{
    sin_family = AF_INET;
    sin_port = AnyPort;
    sin_addr = address;
    std::fill(std::begin(sin_zero), std::end(sin_zero), char {0});
}

SockAddrIPV4::SockAddrIPV4(const in_addr & address, PortType port)
    : sockaddr_in()
{
    sin_family = AF_INET;
    sin_port = port;
    sin_addr = address;
    std::fill(std::begin(sin_zero), std::end(sin_zero), char {0});
}

SockAddrIPV4 & SockAddrIPV4::operator = (const SockAddrIPV4 & other)
{
    if (&other != this)
    {
        sin_family = other.sin_family;
        sin_port = other.sin_port;
        sin_addr = other.sin_addr;
        std::fill(std::begin(sin_zero), std::end(sin_zero), char {0});
    }
    return *this;
}

SockAddrIPV4 & SockAddrIPV4::operator = (const sockaddr_in & other)
{
    sin_family = other.sin_family;
    sin_port = other.sin_port;
    sin_addr = other.sin_addr;
    std::fill(std::begin(sin_zero), std::end(sin_zero), char {0});
    return *this;
}

SockAddrIPV4 & SockAddrIPV4::operator = (const in_addr & address)
{
    sin_family = AF_INET;
    sin_port = AnyPort;
    sin_addr = address;
    std::fill(std::begin(sin_zero), std::end(sin_zero), char {0});
    return *this;
}

SocketFamily SockAddrIPV4::family() const
{
    return static_cast<SocketFamily>(sin_family);
}

in_addr SockAddrIPV4::address() const
{
    return sin_addr;
}

PortType SockAddrIPV4::port() const
{
    return sin_port;
}

IPV4EndPoint::IPV4EndPoint()
    : m_ipAddress()
    , m_port(AnyPort)
{
}
IPV4EndPoint::IPV4EndPoint(const IPV4EndPoint & other)
    : m_ipAddress(other.m_ipAddress)
    , m_port(other.m_port)
{
}
IPV4EndPoint::IPV4EndPoint(const IPV4Address & ipAddress)
    : m_ipAddress(ipAddress)
    , m_port(AnyPort)
{
}
IPV4EndPoint::IPV4EndPoint(const IPV4Address & ipAddress, PortType port)
    : m_ipAddress(ipAddress)
    , m_port(port)
{
}
IPV4EndPoint::IPV4EndPoint(const IPV4Address::AddressType & ipAddress)
    : m_ipAddress(ipAddress)
    , m_port(AnyPort)
{
}
IPV4EndPoint::IPV4EndPoint(const IPV4Address::AddressType & ipAddress, PortType port)
    : m_ipAddress(ipAddress)
    , m_port(port)
{
}
IPV4EndPoint::IPV4EndPoint(const in_addr & ipAddress)
    : m_ipAddress(ipAddress)
    , m_port(AnyPort)
{
}
IPV4EndPoint::IPV4EndPoint(const in_addr & ipAddress, PortType port)
    : m_ipAddress(ipAddress)
    , m_port(port)
{
}

IPV4EndPoint::IPV4EndPoint(PortType port)
    : m_ipAddress(IPV4Address::None)
    , m_port(port)
{
}
IPV4EndPoint::IPV4EndPoint(const SockAddrIPV4 & address)
    : IPV4EndPoint(address.address(), address.port())
{
}
IPV4EndPoint::~IPV4EndPoint()
{
}

IPV4EndPoint & IPV4EndPoint::operator = (const IPV4EndPoint & other)
{
    if (&other != this)
    {
        m_ipAddress = other.m_ipAddress;
        m_port = other.m_port;
    }
    return *this;
}

IPV4EndPoint IPV4EndPoint::Parse(const std::string & text)
{
    IPV4EndPoint ipAddress;
    if (!TryParse(text, ipAddress))
    {
        tracing::Tracer::Throw(__FILE__, __LINE__, __func__, 
            utility::GenericError("IPV4Address string representation must be formatted as ddd.ddd.ddd.ddd[:ddddd]"
                                  ", string is {}", text));
    }
    return ipAddress;
}

bool IPV4EndPoint::TryParse(const std::string & text, IPV4EndPoint & ipEndPoint)
{
    std::size_t pos = text.find(':');
    IPV4Address address = IPV4Address::None;
    std::uint16_t port = AnyPort;
    if (pos == std::string::npos)
    {
        if (!IPV4Address::TryParse(text, address))
            return false;
    }
    else
    {
        if (!IPV4Address::TryParse(text.substr(0, pos), address))
            return false;
        if (!serialization::Deserialize(text.substr(pos + 1), port, 10))
            return false;
    }
    ipEndPoint = IPV4EndPoint(address, static_cast<uint16_t>(port));
    return true;
}

SockAddrIPV4 IPV4EndPoint::ConvertAddress() const
{
    return SockAddrIPV4(IPAddress().ConvertAddress(), utility::ToNetworkByteOrder(Port()));
}

bool IPV4EndPoint::operator == (const IPV4EndPoint & other) const
{
    if (&other == this)
        return true;
    return (other.m_ipAddress == m_ipAddress) && (other.m_port == m_port);
}

bool IPV4EndPoint::operator != (const IPV4EndPoint & other) const
{
    return ! this->operator ==(other);
}

std::istream & operator >> (std::istream & stream, IPV4EndPoint & value)
{
    std::string text;
    stream >> text;
    if (!stream.fail())
    {
        if (!IPV4EndPoint::TryParse(text, value))
            stream.clear(stream.failbit);
    }
    return stream;
}

std::ostream & operator << (std::ostream & stream, const IPV4EndPoint & value)
{
    return stream << value.IPAddress() << ":" << value.Port();
}

} // namespace network