#pragma once

#include <array>
#include <iostream>

namespace network {

class DomainSocketAddress
{
public:
    static constexpr std::size_t AddressSize = 108;
    using AddressType = std::array<std::uint8_t, AddressSize>;

private:
    AddressType m_address;

public:
    static const DomainSocketAddress Any;
    static const DomainSocketAddress None;
    static const DomainSocketAddress Broadcast;
    static const DomainSocketAddress LocalHost;

    DomainSocketAddress();
    DomainSocketAddress(const DomainSocketAddress & other);
    DomainSocketAddress(const AddressType & address);
    DomainSocketAddress(const std::initializer_list<std::uint8_t> & address);
    DomainSocketAddress(const std::string & address);
    virtual ~DomainSocketAddress();

    DomainSocketAddress & operator = (const DomainSocketAddress & other);

    static DomainSocketAddress Parse(const std::string & text);
    static bool TryParse(const std::string & text, DomainSocketAddress & address);

    AddressType Address() const { return m_address; }

    friend bool operator == (const DomainSocketAddress & lhs, const DomainSocketAddress & rhs);
    friend bool operator == (const AddressType & lhs, const DomainSocketAddress & rhs);
    friend bool operator == (const DomainSocketAddress & lhs, const AddressType & rhs);
    friend bool operator != (const DomainSocketAddress & lhs, const DomainSocketAddress & rhs);
    friend bool operator != (const AddressType & lhs, const DomainSocketAddress & rhs);
    friend bool operator != (const DomainSocketAddress & lhs, const AddressType & rhs);
};

std::istream & operator >> (std::istream & stream, DomainSocketAddress & value);
std::ostream & operator << (std::ostream & stream, const DomainSocketAddress & value);

} // namespace network
