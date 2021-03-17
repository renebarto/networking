//---------------------------------------------------------------
// Copyright   : Copyright(c) 2020, Koninklijke Philips Electronics N.V.
//
// File        : Serialize.h
//
// Namespace   : core::serialization
//
// Class       : -
//
//
// Description :
//  Type serialization
//  Enumeration of standard enums
//
//---------------------------------------------------------------

#pragma once

//TICS -POR#021 We suppress warnings for Windows only
#if defined(PLATFORM_WINDOWS)
#pragma warning(disable : 4647) //TICS !POR#018 !POR#037 behavior change: __is_pod
#include <chrono>
#pragma warning(default : 4647) //TICS !POR#018 !POR#037
#endif
//TICS +POR#021
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>
#include "osal/Clock.h"
#include "core/types/CommonTypes.h"

namespace core {
namespace serialization {

std::string ToStringImpl(core::types::XRayChannel value);
std::string ToStringImpl(core::types::LoggingSeverity value);

} // namespace serialization
} // namespace core

template <class T>
typename std::enable_if<
    !std::is_same<T, char*>::value &&
    !std::is_same<T, const char*>::value &&
    !std::is_pod<T>::value, std::string>::type
ToString(const T & value)
{
    return std::to_string(value);
}

template <class T>
typename std::enable_if<
    std::is_same<T, char*>::value ||
    std::is_same<T, const char*>::value ||
    std::is_pod<T>::value, std::string>::type
ToString(T value)
{
    return std::to_string(value);
}

// Use the ToStringHex to convert a hexadecimal value to a string.
// The string will be in the format "0x FF FF FF"
// If the "0x " is unwanted then use the STRHEX_NOBASE macro
template <typename T>
std::string ToStringHex(const T& obj)
{
    std::ostringstream str;
    try
    {
        str << "0x" << std::hex << std::noshowbase << std::uppercase << std::setw(8) << std::setfill('0') << static_cast<std::uint64_t>(obj);
    }
    catch (...)
    {
    }
    return str.str();
}

template <typename T>
std::string ToStringHexNoBase(const T& obj)
{
    std::ostringstream str;
    try
    {
        str << std::hex << std::noshowbase << std::uppercase << std::setw(8) << std::setfill('0') << static_cast<std::uint64_t>(obj);
    }
    catch (...)
    {
    }
    return str.str();
}

template <typename T>
std::string ToStringPtr(const T & obj)
{
    std::ostringstream str;
    try
    {
//TICS -POR#021 We suppress warnings for Windows only
#if defined(PLATFORM_WINDOWS)
#pragma warning(disable : 4826) //TICS !POR#018 !POR#037 Conversion is sign extended
#endif
        str << "0x" << std::hex << std::noshowbase << std::uppercase << std::setw(16) << std::setfill('0') << reinterpret_cast<std::uint64_t>(obj); //TICS !CON#006 !CON#001 Allow reinterpret_cast
#if defined(PLATFORM_WINDOWS)
#pragma warning(default : 4826) //TICS !POR#018 !POR#037
#endif
//TICS +POR#021
    }
    catch (...)
    {
    }
    return str.str();
}

template <>
inline std::string ToString(const char * value)
{
    return std::string(value);
}

template <>
inline std::string ToString(bool value)
{
    return value ? "true" : "false";
}

template <>
inline std::string ToString(osal::EpochTime value)
{
    return osal::Clock::ToString(value);
}

template <>
inline std::string ToString(const osal::Clock & value)
{
    return osal::Clock::ToString(value.CurrentTime());
}
