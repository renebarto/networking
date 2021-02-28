//------------------------------------------------------------------------------
// Copyright   : Copyright(c) 2020, Rene Barto
//
// File        : SoundAPI.h
//
// Namespace   : midi
//
// Class       : -
//
// Description : 
//
//------------------------------------------------------------------------------

#pragma once

#if defined(PLATFORM_WINDOWS)

#include <map>
#include <string>
#include <vector>
#include "sound/ISoundAPI.h"

#include <windows.h>

namespace sound {

class SoundAPIWindows
    : public ISoundAPI
{
private:
    bool m_isInitialized;

public:
    SoundAPIWindows();
    ~SoundAPIWindows();

    bool Initialize() override;
    void Uninitialize() override;
    bool IsInitialized() override;
};

} // namespace sound

#endif