//------------------------------------------------------------------------------
// Copyright   : Copyright(c) 2020, Rene Barto
//
// File        : SoundAPIWindows.cpp
//
// Namespace   : midi
//
// Class       : -
//
// Description : 
//
//------------------------------------------------------------------------------

#if defined(PLATFORM_WINDOWS)

#include "tracing/ScopedTracing.h"
#include "utility/EnumSerialization.h"

#include "SoundAPIWindows.h"

namespace sound {

ISoundAPIPtr CreateAPI()
{
    SCOPEDTRACE(nullptr, nullptr);
    return std::make_unique<SoundAPIWindows>();
}

SoundAPIWindows::SoundAPIWindows()
    : m_isInitialized()
{
    SCOPEDTRACE(
        nullptr, 
        nullptr);
}

SoundAPIWindows::~SoundAPIWindows()
{
    SCOPEDTRACE(
        nullptr, 
        nullptr);
    Uninitialize();
}

bool SoundAPIWindows::Initialize()
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

void SoundAPIWindows::Uninitialize()
{
    SCOPEDTRACE(
        nullptr, 
        nullptr);
    m_isInitialized = false;
}

bool SoundAPIWindows::IsInitialized()
{
    return m_isInitialized;
}

} // namespace sound

#endif
