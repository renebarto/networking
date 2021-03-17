//------------------------------------------------------------------------------
// Copyright   : Copyright(c) 2020, Rene Barto
//
// File        : PropertyVariant.cpp
//
// Namespace   : sound
//
// Class       : -
//
// Description : 
//
//------------------------------------------------------------------------------

#if defined(PLATFORM_WINDOWS)

#include <MMDeviceAPI.h>

#include "tracing/ScopedTracing.h"

#include "WindowsCOM.h"
#include "PropertyVariant.h"

namespace sound {

PropertyVariant::PropertyVariant()
    : m_variant()
{
    SCOPEDTRACE(
        nullptr, 
        nullptr);
    PropVariantInit(&m_variant);
}

PropertyVariant::~PropertyVariant()
{
    SCOPEDTRACE(
        nullptr, 
        nullptr);
    PropVariantClear(&m_variant);
}

PROPVARIANT & PropertyVariant::Get()
{
    SCOPEDTRACE(
        nullptr, 
        nullptr);
    return m_variant;
}

} // namespace sound

#endif
