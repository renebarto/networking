//------------------------------------------------------------------------------
// Copyright   : Copyright(c) 2021 Koninklijke Philips Electronics N.V.
//
// File        : Utilities.h
//
// Namespace   : osal
//
// Class       : -
//
// Description :
//
//------------------------------------------------------------------------------

#pragma once

#include <cstdint>

//TICS -POR#021 We suppress warnings for Windows only
#if defined(PLATFORM_LINUX)

#include <byteswap.h>

#endif
//TICS +POR#021

namespace osal {

template<class T> T SwapBytes(T value);

template<> inline std::int8_t SwapBytes<std::int8_t>(std::int8_t value)
{
    return value;
}
template<> inline std::uint8_t SwapBytes<std::uint8_t>(std::uint8_t value)
{
    return value;
}
template<> inline std::int16_t SwapBytes<std::int16_t>(std::int16_t value)
{
//TICS -CFL#020 Return statement
//TICS -POR#021 Platform specific
#if defined(PLATFORM_WINDOWS)
    return static_cast<std::int16_t>(_byteswap_ushort(static_cast<std::uint16_t>(value)));
#elif defined(PLATFORM_LINUX)
    return static_cast<std::int16_t>(bswap_16(value)); //TICS !NAM#008 !CON#004 !ORG#013 bswap_16 uses cast and function starting with __
#endif
//TICS +POR#021
//TICS +CFL#020 Return statement
}
template<> inline std::uint16_t SwapBytes<std::uint16_t>(std::uint16_t value)
{
//TICS -POR#021 Platform specific
#if defined(PLATFORM_WINDOWS)
    return _byteswap_ushort(value);
#elif defined(PLATFORM_LINUX)
    return bswap_16(value); //TICS !NAM#008 !CON#004 !ORG#013 bswap_16 uses cast and function starting with __
#endif
//TICS +POR#021
}
template<> inline std::int32_t SwapBytes<std::int32_t>(std::int32_t value)
{
//TICS -POR#021 Platform specific
#if defined(PLATFORM_WINDOWS)
    return static_cast<std::int32_t>(_byteswap_ulong(static_cast<std::uint32_t>(value)));
#elif defined(PLATFORM_LINUX)
    return static_cast<std::int32_t>(bswap_32(value));
#endif
//TICS +POR#021
}
template<> inline std::uint32_t SwapBytes<std::uint32_t>(std::uint32_t value)
{
//TICS -POR#021 Platform specific
#if defined(PLATFORM_WINDOWS)
    return _byteswap_ulong(value);
#elif defined(PLATFORM_LINUX)
    return bswap_32(value);
#endif
//TICS +POR#021
}
template<> inline std::int64_t SwapBytes<std::int64_t>(std::int64_t value)
{
//TICS -POR#021 Platform specific
#if defined(PLATFORM_WINDOWS)
    return static_cast<std::int64_t>(_byteswap_uint64(static_cast<std::uint64_t>(value)));
#elif defined(PLATFORM_LINUX)
    return static_cast<std::int64_t>(bswap_64(value));
#endif
//TICS +POR#021
}
template<> inline std::uint64_t SwapBytes<std::uint64_t>(std::uint64_t value)
{
//TICS -POR#021 Platform specific
#if defined(PLATFORM_WINDOWS)
    return _byteswap_uint64(value);
#elif defined(PLATFORM_LINUX)
    return bswap_64(value);
#endif
//TICS +POR#021
}

//TICS -POR#021 Platform specific
#if !defined (PLATFORM_LINUX)
template<> inline unsigned long SwapBytes<unsigned long>(unsigned long value)
{
    return SwapBytes<std::uint32_t>(value);
}
#endif
//TICS +POR#021

} // namespace osal
