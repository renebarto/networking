#pragma once

#include "network/MACAddress.h"
#include "network/IPV4Address.h"
#include "network/IPV6Address.h"

namespace network {

enum class AddressType
{
    Packet,
    IPV4,
    IPV6,
    Invalid,
};

class AddressTuple
{
private:
    AddressType m_type;
    MACAddress m_macAddress;
    IPV4Address m_ipV4Address;
    IPV6Address m_ipV6Address;
    friend std::ostream & operator << (std::ostream & stream, const AddressTuple & value);

public:
    AddressTuple()
        : m_type(AddressType::Invalid)
        , m_macAddress()
        , m_ipV4Address()
        , m_ipV6Address()
    {}
    AddressTuple(AddressType type)
        : m_type(type)
        , m_macAddress()
        , m_ipV4Address()
        , m_ipV6Address()
    {}
    AddressTuple(const MACAddress & address)
        : m_type(AddressType::Packet)
        , m_macAddress(address)
        , m_ipV4Address()
        , m_ipV6Address()
    {}
    AddressTuple(const IPV4Address & address)
        : m_type(AddressType::IPV4)
        , m_macAddress()
        , m_ipV4Address(address)
        , m_ipV6Address()
    {}
    AddressTuple(const IPV6Address & address)
        : m_type(AddressType::IPV6)
        , m_macAddress()
        , m_ipV4Address()
        , m_ipV6Address(address)
    {}

    bool IsValid() const { return m_type != AddressType::Invalid; }
    bool IsMACAddress() const { return m_type == AddressType::Packet; }
    bool IsIPV4Address() const { return m_type == AddressType::IPV4; }
    bool IsIPV6Address() const { return m_type == AddressType::IPV6; }
    const MACAddress & GetMACAddress() const { return m_macAddress; }
    const IPV4Address & GetIPV4Address() const { return m_ipV4Address; }
    const IPV6Address & GetIPV6Address() const { return m_ipV6Address; }
};

std::ostream & operator << (std::ostream & stream, const AddressTuple & value);

} // namespace network
