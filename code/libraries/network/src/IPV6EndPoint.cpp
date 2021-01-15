#include "network/IPV6EndPoint.h"

#include "utility/Endian.h"
#include "utility/GenericError.h"
#include "tracing/Logging.h"

namespace network {

SockAddrIPV6::SockAddrIPV6()
    : sockaddr_in6()
{
}

SockAddrIPV6::SockAddrIPV6(const SockAddrIPV6 & other)
    : sockaddr_in6(other)
{
}

SockAddrIPV6::SockAddrIPV6(const sockaddr_in6 & other)
    : sockaddr_in6(other)
{
}

SockAddrIPV6::SockAddrIPV6(const in6_addr & address)
    : sockaddr_in6()
{
    sin6_family = AF_INET6;
    sin6_port = AnyPort;
    sin6_addr = address;
    sin6_flowinfo = 0;
    sin6_scope_id = 0;
}

SockAddrIPV6::SockAddrIPV6(const in6_addr & address, PortType port)
    : sockaddr_in6()
{
    sin6_family = AF_INET6;
    sin6_port = port;
    sin6_addr = address;
    sin6_flowinfo = 0;
    sin6_scope_id = 0;
}

SockAddrIPV6::SockAddrIPV6(const in6_addr & address, PortType port, uint32_t flowinfo, uint32_t scopeid)
    : sockaddr_in6()
{
    sin6_family = AF_INET6;
    sin6_port = port;
    sin6_addr = address;
    sin6_flowinfo = flowinfo;
    sin6_scope_id = scopeid;
}

SockAddrIPV6 & SockAddrIPV6::operator = (const SockAddrIPV6 & other)
{
    if (&other != this)
    {
        sin6_family = other.sin6_family;
        sin6_port = other.sin6_port;
        sin6_addr = other.sin6_addr;
        sin6_flowinfo = other.sin6_flowinfo;
        sin6_scope_id = other.sin6_scope_id;
    }
    return *this;
}

SockAddrIPV6 & SockAddrIPV6::operator = (const sockaddr_in6 & other)
{
    sin6_family = other.sin6_family;
    sin6_port = other.sin6_port;
    sin6_addr = other.sin6_addr;
    sin6_flowinfo = other.sin6_flowinfo;
    sin6_scope_id = other.sin6_scope_id;
    return *this;
}

SockAddrIPV6 & SockAddrIPV6::operator = (const in6_addr & address)
{
    sin6_family = AF_INET6;
    sin6_port = AnyPort;
    sin6_addr = address;
    sin6_flowinfo = 0;
    sin6_scope_id = 0;
    return *this;
}

SocketFamily SockAddrIPV6::family() const
{
    return static_cast<SocketFamily>(sin6_family);
}

in6_addr SockAddrIPV6::address() const
{
    return sin6_addr;
}

PortType SockAddrIPV6::port() const
{
    return sin6_port;
}

uint32_t SockAddrIPV6::flowinfo() const
{
    return sin6_flowinfo;
}

uint32_t SockAddrIPV6::scopeid() const
{
    return sin6_scope_id;
}

IPV6EndPoint::IPV6EndPoint()
    : m_ipAddress()
    , m_port(AnyPort)
    , m_flowInformation()
    , m_scopeIdentifier()

{
}
IPV6EndPoint::IPV6EndPoint(const IPV6EndPoint & other)
    : m_ipAddress(other.m_ipAddress)
    , m_port(other.m_port)
    , m_flowInformation(other.m_flowInformation)
    , m_scopeIdentifier(other.m_scopeIdentifier)
{
}
IPV6EndPoint::IPV6EndPoint(const IPV6Address & ipAddress)
    : m_ipAddress(ipAddress)
    , m_port(AnyPort)
    , m_flowInformation()
    , m_scopeIdentifier()
{
}
IPV6EndPoint::IPV6EndPoint(const IPV6Address & ipAddress, PortType port,
                           uint32_t flowInformation, uint32_t scopeIdentifier)
    : m_ipAddress(ipAddress)
    , m_port(port)
    , m_flowInformation(flowInformation)
    , m_scopeIdentifier(scopeIdentifier)
{
}
IPV6EndPoint::IPV6EndPoint(const in6_addr & ipAddress)
        : m_ipAddress(ipAddress)
        , m_port(AnyPort)
        , m_flowInformation()
        , m_scopeIdentifier()
{
}
IPV6EndPoint::IPV6EndPoint(const in6_addr & ipAddress, PortType port,
                           uint32_t flowInformation, uint32_t scopeIdentifier)
        : m_ipAddress(ipAddress)
        , m_port(port)
        , m_flowInformation(flowInformation)
        , m_scopeIdentifier(scopeIdentifier)
{
}
IPV6EndPoint::IPV6EndPoint(const IPV6Address::AddressType ipAddress, PortType port,
                           uint32_t flowInformation, uint32_t scopeIdentifier)
    : m_ipAddress(ipAddress)
    , m_port(port)
    , m_flowInformation(flowInformation)
    , m_scopeIdentifier(scopeIdentifier)
{
}
IPV6EndPoint::IPV6EndPoint(PortType port)
    : m_ipAddress(IPV6Address::None)
    , m_port(port)
    , m_flowInformation()
    , m_scopeIdentifier()
{
}
IPV6EndPoint::IPV6EndPoint(const SockAddrIPV6 & address)
    : m_ipAddress(address.address())
    , m_port(utility::FromNetworkByteOrder(address.port()))
    , m_flowInformation(utility::FromNetworkByteOrder(address.flowinfo()))
    , m_scopeIdentifier(utility::FromNetworkByteOrder(address.scopeid()))
{
}

IPV6EndPoint & IPV6EndPoint::operator = (const IPV6EndPoint & other)
{
    if (&other != this)
    {
        m_ipAddress = other.m_ipAddress;
        m_port = other.m_port;
        m_flowInformation = other.m_flowInformation;
        m_scopeIdentifier = other.m_scopeIdentifier;
    }
    return *this;
}

IPV6EndPoint IPV6EndPoint::Parse(const std::string & text)
{
    IPV6EndPoint ipAddress;
    if (!TryParse(text, ipAddress))
    {
        tracing::Logging::Throw(__FILE__, __LINE__, __func__, 
            utility::GenericError("IPV6EndPoint string representation must be formatted as "
                  "[xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx:xxxx]:ddddd (or a shortened format),  string is {}", text));
    }
    return ipAddress;
}

static std::uint32_t ParseScopeID(const std::string & text)
{
    std::uint32_t value = 0;
    for (auto ch : text)
    {
        if (!std::isdigit(ch))
            break;
        value = value * 10 + (ch - '0');
    }
    return value;
}

bool IPV6EndPoint::TryParse(const std::string & text, IPV6EndPoint & ipEndPoint)
{
    IPV6Address address = IPV6Address::None;
    std::uint16_t port = AnyPort;
    std::uint32_t scopeID = 0;
    size_t bracketPos = text.find('[');
    if (bracketPos == 0)
    {
        // We have a URI with possibly a port number
        bracketPos = text.find(']');
        if (bracketPos == std::string::npos)
            return false;
        size_t scopeDelimiterPos = text.find('%', 1);
        if (scopeDelimiterPos == std::string::npos)
        {
            if (!IPV6Address::TryParse(text.substr(1, bracketPos - 1), address))
                return false;
            size_t pos = text.find(':', bracketPos + 1);
            if (pos != std::string::npos)
            {
                if (!serialization::Deserialize(text.substr(pos + 1), port, 10))
                    return false;
            }
        }
        else
        {
            if (!IPV6Address::TryParse(text.substr(1, bracketPos - 1), address))
                return false;
            scopeID = ParseScopeID(text.substr(scopeDelimiterPos + 1, bracketPos));
            size_t pos = text.find(':', bracketPos + 1);
            if (pos != std::string::npos)
            {
                if (!serialization::Deserialize(text.substr(pos + 1, scopeDelimiterPos - pos - 1), port, 10))
                    return false;
            }
        }
    }
    else
    {
        // We have a plain IPV6 address with possibly a scope identifier
        size_t scopeDelimiterPos = text.find('%');
        if (scopeDelimiterPos == std::string::npos)
        {
            // We have a plain IPV6 address
            if (!IPV6Address::TryParse(text, address))
                return false;
        }
        else
        {
            // We have a plain IPV6 address with a scope identifier
            if (!IPV6Address::TryParse(text.substr(0, scopeDelimiterPos), address))
                return false;
            scopeID = ParseScopeID(text.substr(scopeDelimiterPos + 1));
        }
    }
    ipEndPoint = IPV6EndPoint(address, static_cast<uint16_t>(port), 0, scopeID);
    return true;
}

SockAddrIPV6 IPV6EndPoint::ConvertAddress() const
{
    return SockAddrIPV6(
        IPAddress().ConvertAddress(), 
        utility::ToNetworkByteOrder(Port()),
        utility::ToNetworkByteOrder(FlowInfo()),
        utility::ToNetworkByteOrder(ScopeID()));
}

bool IPV6EndPoint::operator == (const IPV6EndPoint & other) const
{
    if (&other == this)
        return true;
    return (other.m_ipAddress == m_ipAddress) && (other.m_port == m_port) &&
           (other.m_flowInformation == m_flowInformation) && (other.m_scopeIdentifier == m_scopeIdentifier);
}

bool IPV6EndPoint::operator != (const IPV6EndPoint & other) const
{
    return ! this->operator ==(other);
}

std::istream & operator >> (std::istream & stream, IPV6EndPoint & value)
{
    std::string text;
    stream >> text;
    if (!stream.fail())
    {
        if (!IPV6EndPoint::TryParse(text, value))
            stream.clear(stream.failbit);
    }
    return stream;
}

std::ostream & operator << (std::ostream & stream, const IPV6EndPoint & value)
{
    if (value.Port() != 0)
    {
        stream << "[" << value.IPAddress() << "]:" << value.Port();
    }
    else
    {
        stream << value.IPAddress();
    }
    if (value.ScopeID() != 0)
    {
        stream << '%' << value.ScopeID();
    }
    return stream;
}

} // namespace network
