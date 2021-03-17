//---------------------------------------------------------------
// Copyright   : Copyright(c) 2020, Koninklijke Philips Electronics N.V.
//
// File        : CommonTypes.cpp
//
// Namespace   : serialization
//
// Class       : -
//
// Description :
//      Type serialization
//
//---------------------------------------------------------------

#include "core/types/CommonTypes.h"

namespace serialization {

template<>
const utility::BidirectionalMap<core::types::LoggingSeverity, std::string> EnumSerializationMap<core::types::LoggingSeverity>::ConversionMap = 
{
    { core::types::LoggingSeverity::Fatal, "Fatal" },
    { core::types::LoggingSeverity::Error, "Error" },
    { core::types::LoggingSeverity::Warning, "Warning" },
    { core::types::LoggingSeverity::Info, "Info" },
};

template<>
const utility::BidirectionalMap<core::types::XRayChannel, std::string> EnumSerializationMap<core::types::XRayChannel>::ConversionMap = 
{
    { core::types::XRayChannel::FRONTAL, "Frontal" },
    { core::types::XRayChannel::LATERAL, "Lateral" },
};

} // namespace serialization
