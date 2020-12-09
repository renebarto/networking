#include "osal/StringConversion.h"

#include <codecvt>
#include <locale>

#if defined(PLATFORM_WINDOWS)

#pragma warning(disable: 5039)
#include <windows.h>
#pragma warning(default: 5039)

#elif defined(PLATFORM_LINUX)

#else

#error Unsupported platform

#endif

namespace osal {

std::wstring StringToWString(const std::string & value)
{
#if defined(PLATFORM_WINDOWS)

    uint64_t size = static_cast<uint64_t>(MultiByteToWideChar(CP_UTF8, 0, value.c_str(), static_cast<int>(value.length()), nullptr, int{ 0 }));
    wchar_t *buffer = new wchar_t[static_cast<size_t>(size) + 1];
    std::wstring result;
    try
    {
        MultiByteToWideChar(CP_UTF8, 0, value.c_str(), static_cast<int>(value.length()), buffer, static_cast<int>(size));
        result = std::wstring(buffer, size);
    }
    catch (...)
    {
    }
    delete[] buffer;

    return result;

#elif defined(PLATFORM_LINUX)

    using convert_type = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_type, wchar_t> converter;

    return converter.from_bytes(value);

#endif
}

std::string WStringToString(const std::wstring & value)
{
#if defined(PLATFORM_WINDOWS)

    uint64_t size = static_cast<uint64_t>(WideCharToMultiByte(CP_UTF8, 0, value.c_str(), static_cast<int>(value.length()), nullptr, 0, nullptr, nullptr));
    char* buffer = new char[size + 1];
    std::string result;
    try
    {
        WideCharToMultiByte(CP_UTF8, 0, value.c_str(), static_cast<int>(value.length()), buffer, static_cast<int>(size), nullptr, nullptr);
        result = std::string(buffer, size);
    }
    catch (...)
    {
    }
    delete[] buffer;

    return result;

#elif defined(PLATFORM_LINUX)

    using convert_type = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_type, wchar_t> converter;

    return converter.to_bytes(value);

#endif
}

} // namespace osal