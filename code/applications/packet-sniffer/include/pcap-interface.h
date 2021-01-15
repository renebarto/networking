#pragma once

#include <array>
#include <string>
#include <vector>
#include "network-osal/AddressTuple.h"

namespace packetcapture
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
    Interface(const std::string & name, const std::string & description, bool isLoopback, const std::vector<AddressInfo> & addresses);
    Interface(const Interface &) = default;
    Interface(Interface &&) = default;
    Interface & operator = (const Interface &) = default;
    Interface & operator = (Interface &&) = default;
    ~Interface();

    const std::string & Name() const { return m_name; }
    const std::string & Description() const { return m_description; }
    bool IsLoopback() const { return m_isLoopback; }
    const std::vector<AddressInfo> & Addresses() const { return m_addresses; }

private:
    friend class Interfaces;

    std::string m_name;
    std::string m_description;
    bool m_isLoopback;
    std::vector<AddressInfo> m_addresses;
};

} // namespace packetcapture