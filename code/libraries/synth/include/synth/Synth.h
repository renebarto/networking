//------------------------------------------------------------------------------
// Copyright   : Copyright(c) 2020, Rene Barto
//
// File        : Synth.h
//
// Namespace   : synth
//
// Class       : -
//
// Description : 
//
//------------------------------------------------------------------------------

#pragma once

#include <iostream>
#include <memory>

namespace synth {

class Synth
{
private:
    bool m_isInitialized;

public:
    Synth();
    virtual ~Synth();

    bool Initialize();
    void Uninitialize();
    bool IsInitialized();

    // std::uint16_t GetAudio();
};

} // namespace synth
