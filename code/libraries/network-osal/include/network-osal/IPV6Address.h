#pragma once

#include <array>
#include <iostream>

struct in6_addr;

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

    IPV6Address();
    IPV6Address(const IPV6Address & other);
    explicit IPV6Address(const std::initializer_list<std::uint8_t> & ipAddress);
    IPV6Address(const AddressType & address);
    IPV6Address(const struct in6_addr & address);
    virtual ~IPV6Address();

    IPV6Address & operator = (const IPV6Address & other);

    static IPV6Address Parse(const std::string & text);
    static bool TryParse(const std::string & text, IPV6Address & ipAddress);

    AddressType Address() const { return m_address; }
    in6_addr ConvertAddress() const;

    friend bool operator == (const IPV6Address & lhs, const IPV6Address & rhs);
    friend bool operator == (const AddressType & lhs, const IPV6Address & rhs);
    friend bool operator == (const IPV6Address & lhs, const AddressType & rhs);
    friend bool operator != (const IPV6Address & lhs, const IPV6Address & rhs);
    friend bool operator != (const AddressType & lhs, const IPV6Address & rhs);
    friend bool operator != (const IPV6Address & lhs, const AddressType & rhs);
};

std::istream & operator >> (std::istream & stream, IPV6Address & value);
std::ostream & operator << (std::ostream & stream, const IPV6Address & value);

} // namespace network
