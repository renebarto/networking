//------------------------------------------------------------------------------
// Copyright   : Copyright(c) 2014, Koninklijke Philips Electronics N.V.
//
// File        : CommonTypes.h
//
// Namespace   : core::types
//
// Class       : -
//
// Description :
//
//------------------------------------------------------------------------------

#pragma once

#include <ostream>
#include "utility/EnumSerialization.h"

namespace core {
namespace types {

// X-ray channel indication
enum class XRayChannel
{
    FRONTAL,
    LATERAL
};

// Error severities
enum class LoggingSeverity
{
    Info,
    Warning,
    Error,
    Fatal
};

inline std::ostream & operator << (std::ostream & stream, XRayChannel value)
{
    return stream << serialization::Serialize(value);
}

inline std::ostream & operator << (std::ostream & stream, LoggingSeverity value)
{
    return stream << serialization::Serialize(value);
}

} // namespace types
} // namespace core
