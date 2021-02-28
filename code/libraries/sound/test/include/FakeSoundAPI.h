//------------------------------------------------------------------------------
// Copyright   : Copyright(c) 2020, Rene Barto
//
// File        : MidiAPI.h
//
// Namespace   : midi
//
// Class       : -
//
// Description : 
//
//------------------------------------------------------------------------------

#pragma once

#include <map>
#include <string>
#include <vector>

#include "sound/ISoundAPI.h"

namespace sound {

class FakeSoundAPI
    : public ISoundAPI
{
private:
    bool m_isInitialized;

public:
    FakeSoundAPI();
    ~FakeSoundAPI();

    bool Initialize() override;
    void Uninitialize() override;
    bool IsInitialized() override;
};

} // namespace sound
