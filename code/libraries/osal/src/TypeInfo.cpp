//------------------------------------------------------------------------------
// Copyright   : Copyright(c) 2020, Rene Barto
//
// File        : TypeInfo.cpp
//
// Namespace   : osal
//
// Class       : -
//
// Description : Type information abstraction
//
//------------------------------------------------------------------------------

#include "osal/TypeInfo.h"

//TICS -POR#021 Specific Linux includes
#ifdef __GNUG__
#include <cxxabi.h>
#include <cstdlib>
#include <memory>
#endif
//TICS +POR#021

namespace osal {

// Implementations (GNU and other) for demangle function used in the "type" method that returns the name of a class
// Class names are passed on to fatal and log functions as additional debug info

//TICS -POR#021 Platform dependent implementation
#ifdef __GNUG__

std::string demangle(const char* name)
 {
    int status {};
    std::unique_ptr<char, void(*)(void*)> res
    {
        abi::__cxa_demangle(name, NULL, NULL, &status),
        std::free
    };
    return (status == 0) ? res.get() : name;
}

#else

// does nothing if not g++
std::string demangle(const char* name) {
    std::string result = name;
    const std::string callingConvention = "__cdecl";
    auto pos = result.find(callingConvention);
    if (pos != std::string::npos)
    {
        result = result.substr(0, pos) + result.substr(pos + callingConvention.length());
    }
    return result;
}

#endif
//TICS +POR#021

} // namespace osal
