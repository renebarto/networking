#pragma once

#include <cstdint>

#if defined(PLATFORM_LINUX)

#include <byteswap.h>

#endif

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
#if defined(PLATFORM_LINUX)
    return static_cast<std::int16_t>(bswap_16(value));
#endif
}
template<> inline std::uint16_t SwapBytes<std::uint16_t>(std::uint16_t value)
{
#if defined(PLATFORM_LINUX)
    return bswap_16(value);
#endif
}
template<> inline std::int32_t SwapBytes<std::int32_t>(std::int32_t value)
{
#if defined(PLATFORM_LINUX)
    return static_cast<std::int32_t>(bswap_32(value));
#endif
}
template<> inline std::uint32_t SwapBytes<std::uint32_t>(std::uint32_t value)
{
#if defined(PLATFORM_LINUX)
    return bswap_32(value);
#endif
}
template<> inline std::int64_t SwapBytes<std::int64_t>(std::int64_t value)
{
#if defined(PLATFORM_LINUX)
    return static_cast<std::int64_t>(bswap_64(value));
#endif
}
template<> inline std::uint64_t SwapBytes<std::uint64_t>(std::uint64_t value)
{
#if defined(PLATFORM_LINUX)
    return bswap_64(value);
#endif
}

} // namespace osal
