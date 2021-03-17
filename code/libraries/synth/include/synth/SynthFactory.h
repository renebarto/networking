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

#include "synth/ISynth.h"

namespace synth {

class SynthFactory
{
public:
    SynthFactory();
    virtual ~SynthFactory();

    ISynthPtr Create(const std::string & type);
};

} // namespace synth
