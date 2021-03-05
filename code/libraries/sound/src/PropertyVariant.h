//------------------------------------------------------------------------------
// Copyright   : Copyright(c) 2020, Rene Barto
//
// File        : PropertyVariant.h
//
// Namespace   : sound
//
// Class       : -
//
// Description : 
//
//------------------------------------------------------------------------------

#pragma once

#if defined(PLATFORM_WINDOWS)

namespace sound {

class PropertyVariant
{
private:
    PROPVARIANT m_variant;

public:
    PropertyVariant();
    ~PropertyVariant();

    PROPVARIANT & Get();
};

} // namespace sound

#endif