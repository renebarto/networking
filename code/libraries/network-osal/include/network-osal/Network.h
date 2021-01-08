#pragma once

#include <cstring>
#include <string>

#if defined(PLATFORM_LINUX) || defined(PLATFORM_LINUX_RPI)

#include <cerrno>

#elif defined(PLATFORM_WINDOWS)

#pragma warning(disable: 5039)
#include <winsock2.h>
#include <iphlpapi.h>
#pragma warning(default: 5039)

#else

#error Unsupported platform

#endif

namespace network {

#if defined(PLATFORM_LINUX) || defined(PLATFORM_LINUX_RPI)

inline int GetError()
{
    return errno;
}

inline std::string GetErrorString(int errorCode)
{
    return std::strerror(errorCode);
}

#elif defined(PLATFORM_WINDOWS)

inline int GetError()
{
    return WSAGetLastError();
}

inline std::string GetErrorString(int errorCode)
{
    char * text {};
    FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, 
        nullptr, static_cast<DWORD>(errorCode),
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        reinterpret_cast<LPSTR>(&text), DWORD {}, nullptr);
    std::string result(text);
    LocalFree(text);
    return result;
}

#endif

} // namespace network