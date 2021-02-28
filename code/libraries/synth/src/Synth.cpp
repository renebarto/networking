//------------------------------------------------------------------------------
// Copyright   : Copyright(c) 2020, Rene Barto
//
// File        : Synth.cpp
//
// Namespace   : synth
//
// Class       : -
//
// Description : 
//
//------------------------------------------------------------------------------

#include "tracing/ScopedTracing.h"
#include "utility/EnumSerialization.h"

#include "synth/Synth.h"

namespace synth {

Synth::Synth()
    : m_isInitialized()
{
    SCOPEDTRACE(
        nullptr, 
        nullptr);
}

Synth::~Synth()
{
    SCOPEDTRACE(
        nullptr, 
        nullptr);
    Uninitialize();
}

bool Synth::Initialize()
{
    bool result {};
    SCOPEDTRACE(
        nullptr, 
        [&] () { return utility::FormatString("result={}", result); });
    if (m_isInitialized)
    {
        result = true;
        return result;
    }

    m_isInitialized = true;
    result = true;
    return result;
}

void Synth::Uninitialize()
{
    SCOPEDTRACE(
        nullptr, 
        nullptr);
    m_isInitialized = false;
}

bool Synth::IsInitialized()
{
    return m_isInitialized;
}

} // namespace synth
