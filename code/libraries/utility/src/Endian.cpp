//------------------------------------------------------------------------------
// Copyright   : Copyright(c) 2020 Koninklijke Philips Electronics N.V.
//
// File        : Endian.cpp
//
// Namespace   : serialization
//
// Class       : -
//
// Description :
//
//------------------------------------------------------------------------------

#include "utility/Endian.h"

namespace serialization {

template<>
const utility::BidirectionalMap<utility::Endianness, std::string> EnumSerializationMap<utility::Endianness>::ConversionMap = 
{
    {utility::Endianness::LittleEndian, "LittleEndian"},
    {utility::Endianness::BigEndian, "BigEndian"},
};

} // namespace serialization
