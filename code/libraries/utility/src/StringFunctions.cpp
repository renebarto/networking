//------------------------------------------------------------------------------
// Copyright   : Copyright(c) 2020 Koninklijke Philips Electronics N.V.
//
// File        : StringFunctions.cpp
//
// Namespace   : utility
//
// Class       : -
//
// Description :
//
//------------------------------------------------------------------------------

#include "utility/StringFunctions.h"

#include <algorithm>
#include <cctype>
#include <cstring>

#if defined(PLATFORM_WINDOWS)

#pragma warning(disable: 5039)
#include <windows.h>
#pragma warning(default: 5039)

#elif defined(PLATFORM_LINUX)

#include <codecvt>
#include <locale>

#else

#error Unsupported platform

#endif

namespace utility {

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

#elif defined(PLATFORM_LINUX) || defined(PLATFORM_LINUX_RPI)

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

#elif defined(PLATFORM_LINUX) || defined(PLATFORM_LINUX_RPI)

    using convert_type = std::codecvt_utf8<wchar_t>;
    std::wstring_convert<convert_type, wchar_t> converter;

    return converter.to_bytes(value);

#endif
}

std::string Align(const std::string & text, int width)
{
    if (static_cast<size_t>(std::abs(width)) > text.length())
    {
        if (width < 0)
        {
            return text + std::string(-width - text.length(), ' ');
        } else
        {
            return std::string(width - text.length(), ' ') + text;
        }
    }
    return text;
}

std::string Trim(const std::string & value, const std::string & stripChars)
{
    const char * find = stripChars.c_str();

    size_t indexLeft = 0;
    while ((indexLeft < value.length()) && strchr(find, value[indexLeft]))
        ++indexLeft;
    size_t indexRight = value.length();
    while ((indexRight > indexLeft) && strchr(find, value[indexRight - 1]))
        --indexRight;
    return value.substr(indexLeft, indexRight - indexLeft);
}

std::string TrimSpaces(const std::string & value)
{
    size_t indexLeft = 0;
    while ((indexLeft < value.length()) && isspace(value[indexLeft]))
        ++indexLeft;
    size_t indexRight = value.length();
    while ((indexRight > indexLeft) && isspace(value[indexRight - 1]))
        --indexRight;
    return value.substr(indexLeft, indexRight - indexLeft);
}

std::string TrimLeft(const std::string & value, const std::string & stripChars)
{
    const char * find = stripChars.c_str();

    size_t indexLeft = 0;
    while ((indexLeft < value.length()) && strchr(find, value[indexLeft]))
        ++indexLeft;
    return value.substr(indexLeft, value.length() - indexLeft);
}

std::string TrimSpacesLeft(const std::string & value)
{
    size_t indexLeft = 0;
    while ((indexLeft < value.length()) && isspace(value[indexLeft]))
        ++indexLeft;
    return value.substr(indexLeft, value.length() - indexLeft);
}

std::string TrimRight(const std::string & value, const std::string & stripChars)
{
    const char * find = stripChars.c_str();

    size_t indexRight = value.length();
    while ((indexRight > 0) && strchr(find, value[indexRight - 1]))
        --indexRight;
    return value.substr(0, indexRight);
}

std::string TrimSpacesRight(const std::string & value)
{
    size_t indexRight = value.length();
    while ((indexRight > 0) && isspace(value[indexRight - 1]))
        --indexRight;
    return value.substr(0, indexRight - 0);
}

std::string ToLower(const std::string & text)
{
    std::string result = text;
    std::transform(result.begin(), result.end(), result.begin(), 
                   [](char c){ return static_cast<char>(std::tolower(static_cast<unsigned char>(c))); });
    return result;
}

std::string ToUpper(const std::string & text)
{
    std::string result = text;
    std::transform(result.begin(), result.end(), result.begin(), 
                   [](char c){ return static_cast<char>(std::toupper(static_cast<unsigned char>(c))); });
    return result;
}

std::string Quote(const std::string & text)
{
    if ((text[0] == '\"') && (text[text.length() - 1] == '\"'))
    {
        return text;
    }
    return "\"" + text + "\"";
}

std::string UnQuote(const std::string & text)
{
    if ((text.length() >= 2) && (text[0] == '\"') && (text[text.length() - 1] == '\"'))
    {
        return text.substr(1, text.length() - 2);
    }
    return text;
}

std::vector<std::string> Split(const std::string & text, char delimiter)
{
    std::size_t pos = 0;
    std::vector<std::string> result;
    std::size_t lastPos = pos;
    while (pos < text.length())
    {
        lastPos = pos;
        pos = text.find(delimiter, pos);
        if (pos != std::string::npos)
        {
            result.push_back(text.substr(lastPos, pos - lastPos));
            ++pos; 
        }
    }
    result.push_back(text.substr(lastPos));
    return result;
}

} // namespace utility