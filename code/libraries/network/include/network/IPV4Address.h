#pragma once

#include <array>
#include <iostream>

namespace network {

class IPV4Address
{
public:
    static constexpr std::size_t AddressSize = 4;
    using AddressType = std::array<std::uint8_t, AddressSize>;

private:
    AddressType m_address;
    friend std::ostream & operator << (std::ostream & stream, const IPV4Address & value);

public:
    static const IPV4Address Any;
    static const IPV4Address None;
    static const IPV4Address Broadcast;
    static const IPV4Address LocalHost;

    IPV4Address()
        : m_address()
    {}
    IPV4Address(const AddressType & address)
        : m_address(address)
    {}
    // Always platform byte order
    IPV4Address(const std::uint32_t address)
        : m_address()
    {
        SetUInt32(address);
    }

    AddressType Address() const { return m_address; }
    // Always platform byte order
    std::uint32_t GetUInt32() const;
    void SetUInt32(std::uint32_t value);

    friend bool operator == (const IPV4Address & lhs, const IPV4Address & rhs);
    friend bool operator == (std::uint32_t lhs, const IPV4Address & rhs);
    friend bool operator == (const IPV4Address & lhs, std::uint32_t rhs);
    friend bool operator == (const AddressType & lhs, const IPV4Address & rhs);
    friend bool operator == (const IPV4Address & lhs, const AddressType & rhs);
    friend bool operator != (const IPV4Address & lhs, const IPV4Address & rhs);
    friend bool operator != (std::uint32_t lhs, const IPV4Address & rhs);
    friend bool operator != (const IPV4Address & lhs, std::uint32_t rhs);
    friend bool operator != (const AddressType & lhs, const IPV4Address & rhs);
    friend bool operator != (const IPV4Address & lhs, const AddressType & rhs);
};

std::ostream & operator << (std::ostream & stream, const IPV4Address & value);

} // namespace network
