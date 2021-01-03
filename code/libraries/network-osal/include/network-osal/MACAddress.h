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

public:
    MACAddress();
    MACAddress(const MACAddress & other);
    MACAddress(const AddressType & address);
    MACAddress(const std::uint8_t address[], std::uint8_t size);
    explicit MACAddress(const std::initializer_list<std::uint8_t> & ipAddress);
    virtual ~MACAddress();

    MACAddress & operator = (const MACAddress & other);

    static MACAddress Parse(const std::string & text);
    static bool TryParse(const std::string & text, MACAddress & macAddress);

    AddressType Address() const { return m_address; }

    friend bool operator == (const MACAddress & lhs, const MACAddress & rhs);
    friend bool operator == (const AddressType & lhs, const MACAddress & rhs);
    friend bool operator == (const MACAddress & lhs, const AddressType & rhs);
    friend bool operator != (const MACAddress & lhs, const MACAddress & rhs);
    friend bool operator != (const AddressType & lhs, const MACAddress & rhs);
    friend bool operator != (const MACAddress & lhs, const AddressType & rhs);
};

std::istream & operator >> (std::istream & stream, MACAddress & value);
std::ostream & operator << (std::ostream & stream, const MACAddress & value);

} // namespace network
