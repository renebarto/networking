#pragma once

#include <array>
#include <iostream>

namespace network {

class IPV6Address
{
public:
    static constexpr std::size_t AddressSize = 16;
    using AddressType = std::array<std::uint8_t, AddressSize>;

private:
    AddressType m_address;
    friend std::ostream & operator << (std::ostream & stream, const IPV6Address & value);

public:
    static const IPV6Address Any;
    static const IPV6Address None;
    static const IPV6Address Broadcast;
    static const IPV6Address LocalHost;

    IPV6Address()
        : m_address()
    {}
    IPV6Address(AddressType address)
        : m_address(address)
    {}
    IPV6Address(const std::uint8_t address[AddressSize])
        : m_address()
    {
        std::copy(address, address + AddressSize, std::begin(m_address));
    }

    AddressType Address() const { return m_address; }

    friend bool operator == (const IPV6Address & lhs, const IPV6Address & rhs);
    friend bool operator == (const AddressType & lhs, const IPV6Address & rhs);
    friend bool operator == (const IPV6Address & lhs, const AddressType & rhs);
    friend bool operator != (const IPV6Address & lhs, const IPV6Address & rhs);
    friend bool operator != (const AddressType & lhs, const IPV6Address & rhs);
    friend bool operator != (const IPV6Address & lhs, const AddressType & rhs);
};

std::ostream & operator << (std::ostream & stream, const IPV6Address & value);

} // namespace network
