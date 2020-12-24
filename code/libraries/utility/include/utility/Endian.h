#pragma once

#include <iostream>

#include "osal/Utilities.h"
#include "utility/EnumSerialization.h"

namespace utility {

enum class Endianness { LittleEndian, BigEndian };

inline std::ostream & operator << (std::ostream & stream, Endianness value)
{
    return stream << serialization::Serialize(value);
}

inline Endianness PlatformEndianness()
{
   const uint16_t value = 0x0001;
   const uint8_t * bytes = reinterpret_cast<const std::uint8_t *>(&value);
   return (bytes[0] == 0x01) ? Endianness::LittleEndian : Endianness::BigEndian;
}

template<typename T>
T FromNetworkByteOrder(T value)
{
    T platformByteOrderValue = value;
    if (PlatformEndianness() == Endianness::LittleEndian)
        platformByteOrderValue = osal::SwapBytes(value);
    return platformByteOrderValue;
}

template<typename T>
T ToNetworkByteOrder(T value)
{
    T networkByteOrderValue = value;
    if (PlatformEndianness() == Endianness::LittleEndian)
        networkByteOrderValue = osal::SwapBytes(value);
    return networkByteOrderValue;
}

} // namespace utility
