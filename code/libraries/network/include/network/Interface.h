#pragma once

#include <array>
#include <string>
#include <vector>
#include "network/AddressTuple.h"

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
    Interface(const std::string & name, bool isUp);
    Interface(const Interface &) = default;
    Interface(Interface &&) = default;
    Interface & operator = (const Interface &) = default;
    Interface & operator = (Interface &&) = default;
    ~Interface();

    const std::string & Name() const { return m_name; }
    bool IsUp() const { return m_isUp; }
    const std::vector<AddressInfo> & Addresses() const { return m_addresses; }
    std::vector<AddressInfo> & Addresses() { return m_addresses; }

private:
    friend class Interfaces;

    std::string m_name;
    bool m_isUp;
    std::vector<AddressInfo> m_addresses;
};

} // namespace network