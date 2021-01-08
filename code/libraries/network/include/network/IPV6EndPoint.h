#pragma once

#include "network-osal/IPV6Address.h"
#include "network-osal/SocketDefinitions.h"

namespace network {

struct SockAddrIPV6 : public sockaddr_in6
{
    SockAddrIPV6();
    SockAddrIPV6(const SockAddrIPV6 & other);
    SockAddrIPV6(const sockaddr_in6 & other);
    SockAddrIPV6(const in6_addr & address);
    SockAddrIPV6(const in6_addr & address, PortType port);
    SockAddrIPV6(const in6_addr & address, PortType port, uint32_t flowinfo, uint32_t scopeid);

    SockAddrIPV6 & operator = (const SockAddrIPV6 & other);
    SockAddrIPV6 & operator = (const sockaddr_in6 & other);
    SockAddrIPV6 & operator = (const in6_addr & address);

    SocketFamily family() const;
    in6_addr address() const;
    PortType port() const;
    uint32_t flowinfo() const;
    uint32_t scopeid() const;
};

class IPV6EndPoint
{
private:
    IPV6Address m_ipAddress;
    PortType m_port;
    uint32_t m_flowInformation;
    uint32_t m_scopeIdentifier;

public:
    static const size_t AddressSize = IPV6Address::AddressSize + sizeof(PortType);

    IPV6EndPoint();
    IPV6EndPoint(const IPV6EndPoint & other);
    IPV6EndPoint(const IPV6Address & ipAddress);
    IPV6EndPoint(const IPV6Address & ipAddress, PortType port,
                 uint32_t flowInformation = 0, uint32_t scopeIdentifier = 0);
    IPV6EndPoint(const in6_addr & ipAddress);
    IPV6EndPoint(const in6_addr & ipAddress, PortType port,
                 uint32_t flowInformation = 0, uint32_t scopeIdentifier = 0);
    IPV6EndPoint(const IPV6Address::AddressType ipAddress, PortType port,
                 uint32_t flowInformation = 0, uint32_t scopeIdentifier = 0);
    IPV6EndPoint(PortType port);
    IPV6EndPoint(const SockAddrIPV6 & address);

    IPV6EndPoint & operator = (const IPV6EndPoint & other);

    static IPV6EndPoint Parse(const std::string & text);
    static bool TryParse(const std::string & text, IPV6EndPoint & ipEndPoint);

    const IPV6Address & IPAddress() const { return m_ipAddress; }
    PortType Port() const { return m_port; }
    uint32_t FlowInfo() const { return m_flowInformation; }
    uint32_t ScopeID() const { return m_scopeIdentifier; }
    size_t Size() const { return sizeof(sockaddr_in6); }
    SockAddrIPV6 ConvertAddress() const;

    bool operator == (const IPV6EndPoint & other) const;
    bool operator != (const IPV6EndPoint & other) const;
};

std::istream & operator >> (std::istream & stream, IPV6EndPoint & value);
std::ostream & operator << (std::ostream & stream, const IPV6EndPoint & value);

} // namespace network
