#pragma once

#include <array>
#include <iostream>

struct in_addr;

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

    IPV4Address();
    IPV4Address(const IPV4Address & other);
    explicit IPV4Address(const std::initializer_list<std::uint8_t> & ipAddress);
    IPV4Address(const AddressType & address);
    // Always network byte order
    IPV4Address(const struct in_addr & address);
    virtual ~IPV4Address();

    IPV4Address & operator = (const IPV4Address & other);

    static IPV4Address Parse(const std::string & text);
    static bool TryParse(const std::string & text, IPV4Address & ipAddress);

    AddressType Address() const { return m_address; }
    // Always network byte order
    std::uint32_t GetUInt32() const;
    void SetUInt32(std::uint32_t value);
    in_addr ConvertAddress() const;

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

std::istream & operator >> (std::istream & stream, IPV4Address & value);
std::ostream & operator << (std::ostream & stream, const IPV4Address & value);

} // namespace network
