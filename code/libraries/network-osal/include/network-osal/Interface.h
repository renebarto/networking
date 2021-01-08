#pragma once

#include <array>
#include <string>
#include <vector>
#include "network-osal/AddressTuple.h"

namespace network
{

struct AddressInfo
{
    network::AddressTuple address;
    network::AddressTuple netmask;
    network::AddressTuple broadcastAddress;
    network::AddressTuple destinationAddress;
};

std::ostream & operator << (std::ostream & stream, const AddressInfo & value);

class Interface
{
public:
    Interface() = default;
    Interface(const std::string & name, bool isUp, bool isLoopback);
    Interface(const Interface &) = default;
    Interface(Interface &&) = default;
    Interface & operator = (const Interface &) = default;
    Interface & operator = (Interface &&) = default;
    ~Interface();

    const std::string & Name() const { return m_name; }
    bool IsUp() const { return m_isUp; }
    bool IsLoopback() const { return m_isLoopback; }
    const std::vector<AddressInfo> & Addresses() const { return m_addresses; }
    std::vector<AddressInfo> & Addresses() { return m_addresses; }
    const std::vector<AddressInfo> & MulticastAddresses() const { return m_multicastAddresses; }
    std::vector<AddressInfo> & MulticastAddresses() { return m_multicastAddresses; }

private:
    friend class Interfaces;

    std::string m_name;
    bool m_isUp;
    bool m_isLoopback;
    std::vector<AddressInfo> m_addresses;
    std::vector<AddressInfo> m_multicastAddresses;
};

std::ostream & operator << (std::ostream & stream, const Interface & value);

} // namespace network