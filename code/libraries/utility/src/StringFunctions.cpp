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

namespace utility {

std::wstring StringToWString(const std::string & value)
{
    return std::wstring(value.begin(), value.end());
}

std::string WStringToString(const std::wstring & value)
{
    return std::string(value.begin(), value.end());
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