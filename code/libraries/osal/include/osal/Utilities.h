#pragma once

#if defined(PLATFORM_LINUX)

#include <byteswap.h>

#endif

namespace osal {

template<class T> T SwapBytes(T value);

template<> inline int8_t SwapBytes<int8_t>(int8_t value)
{
    return value;
}
template<> inline uint8_t SwapBytes<uint8_t>(uint8_t value)
{
    return value;
}
template<> inline int16_t SwapBytes<int16_t>(int16_t value)
{
#if defined(PLATFORM_LINUX)
    return static_cast<int16_t>(bswap_16(value));
#endif
}
template<> inline uint16_t SwapBytes<uint16_t>(uint16_t value)
{
#if defined(PLATFORM_LINUX)
    return bswap_16(value);
#endif
}
template<> inline int32_t SwapBytes<int32_t>(int32_t value)
{
#if defined(PLATFORM_LINUX)
    return static_cast<int32_t>(bswap_32(value));
#endif
}
template<> inline uint32_t SwapBytes<uint32_t>(uint32_t value)
{
#if defined(PLATFORM_LINUX)
    return bswap_32(value);
#endif
}
template<> inline int64_t SwapBytes<int64_t>(int64_t value)
{
#if defined(PLATFORM_LINUX)
    return static_cast<int64_t>(bswap_64(value));
#endif
}
template<> inline uint64_t SwapBytes<uint64_t>(uint64_t value)
{
#if defined(PLATFORM_LINUX)
    return bswap_64(value);
#endif
}


} // namespace osal
