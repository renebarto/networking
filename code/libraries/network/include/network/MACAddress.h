#pragma once

#include <array>
#include <iostream>

namespace network {

class MACAddress
{
public:
    static constexpr std::size_t AddressSize = 6;
    using AddressType = std::array<std::uint8_t, AddressSize>;

private:
    AddressType m_address;
    int m_interfaceIndex;
    friend std::ostream & operator << (std::ostream & stream, const MACAddress & value);

public:
    MACAddress()
        : m_address()
        , m_interfaceIndex()
    {}
    MACAddress(const AddressType & address, int interfaceIndex)
        : m_address(address)
        , m_interfaceIndex(interfaceIndex)
    {}
    MACAddress(const std::uint8_t address[AddressSize], int interfaceIndex)
        : m_address()
        , m_interfaceIndex(interfaceIndex)
    {
        std::copy(address, address + AddressSize, std::begin(m_address));
    }

    AddressType Address() { return m_address; }
    int InterfaceIndex() { return m_interfaceIndex; }

    friend bool operator == (const MACAddress & lhs, const MACAddress & rhs);
    friend bool operator == (const AddressType & lhs, const MACAddress & rhs);
    friend bool operator == (const MACAddress & lhs, const AddressType & rhs);
    friend bool operator != (const MACAddress & lhs, const MACAddress & rhs);
    friend bool operator != (const AddressType & lhs, const MACAddress & rhs);
    friend bool operator != (const MACAddress & lhs, const AddressType & rhs);
};

std::ostream & operator << (std::ostream & stream, const MACAddress & value);

} // namespace network
