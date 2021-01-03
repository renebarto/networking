#pragma once

#include <iostream>
#include "utility/EnumSerialization.h"

namespace network {

enum class EthernetType : unsigned short
{
    IP = 0x0800,
    IPV6 = 0x86dd,
    ARP = 0x0806,
    RARP = 0x8035,
};

inline std::ostream & operator << (std::ostream & stream, EthernetType value)
{
    return stream << serialization::Serialize(value);
}

} // namespace network