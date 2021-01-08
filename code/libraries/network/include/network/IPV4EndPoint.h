#pragma once

#include "network-osal/IPV4Address.h"
#include "network-osal/SocketDefinitions.h"

namespace network {

struct SockAddrIPV4 : public sockaddr_in
{
    SockAddrIPV4();
    SockAddrIPV4(const SockAddrIPV4 & other);
    SockAddrIPV4(const sockaddr_in & other);
    SockAddrIPV4(const in_addr & address);
    SockAddrIPV4(const in_addr & address, PortType port);

    SockAddrIPV4 & operator = (const SockAddrIPV4 & other);
    SockAddrIPV4 & operator = (const sockaddr_in & other);
    SockAddrIPV4 & operator = (const in_addr & address);

    SocketFamily family() const;
    in_addr address() const;
    PortType port() const;
};

class IPV4EndPoint
{
private:
    IPV4Address m_ipAddress;
    PortType m_port;

public:
    static const size_t AddressSize = IPV4Address::AddressSize + sizeof(PortType);

    IPV4EndPoint();
    IPV4EndPoint(const IPV4EndPoint & other);
    explicit IPV4EndPoint(const IPV4Address & ipAddress);
    IPV4EndPoint(const IPV4Address & ipAddress, PortType port);
    explicit IPV4EndPoint(const IPV4Address::AddressType & ipAddress);
    IPV4EndPoint(const IPV4Address::AddressType & ipAddress, PortType port);
    explicit IPV4EndPoint(const in_addr & ipAddress);
    IPV4EndPoint(const in_addr & ipAddress, PortType port);
    IPV4EndPoint(PortType port);
    IPV4EndPoint(const SockAddrIPV4 & address);
    virtual ~IPV4EndPoint();

    IPV4EndPoint & operator = (const IPV4EndPoint & other);

    static IPV4EndPoint Parse(const std::string & text);
    static bool TryParse(const std::string & text, IPV4EndPoint & ipEndPoint);

    const IPV4Address & IPAddress() const { return m_ipAddress; }
    PortType Port() const { return m_port; }
    size_t Size() const { return sizeof(sockaddr_in); }
    SockAddrIPV4 ConvertAddress() const;

    bool operator == (const IPV4EndPoint & other) const;
    bool operator != (const IPV4EndPoint & other) const;
};

std::istream & operator >> (std::istream & stream, IPV4EndPoint & value);
std::ostream & operator << (std::ostream & stream, const IPV4EndPoint & value);

} // namespace network
