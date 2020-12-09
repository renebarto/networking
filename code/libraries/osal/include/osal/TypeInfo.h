//------------------------------------------------------------------------------
// Copyright   : Copyright(c) 2020, Rene Barto
//
// File        : TypeInfo.h
//
// Class       : osal
//
// Description : Type information abstraction
//
//------------------------------------------------------------------------------

#pragma once

#include <ostream>
#include <string>
#include <typeinfo>

namespace osal {

std::string demangle(const char* name);

template <class T>
std::string type(const T& t)
{
//TICS -POR#021 We suppress warnings for Windows only
#if defined (PLATFORM_WINDOWS)
    (void)t; // Windows will otherwise issue warning
#endif
//TICS +POR#021
    std::string name = demangle(typeid(t).name());
    // strip type qualifier "class " from the name
    const std::string classPrefix = "class ";
    auto classPrefixIndex = name.find(classPrefix);
    if (classPrefixIndex != std::string::npos)
        return name.substr(classPrefixIndex + classPrefix.length());
    return name;
}

} // namespace osal
