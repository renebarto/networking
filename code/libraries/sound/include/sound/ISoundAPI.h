//------------------------------------------------------------------------------
// Copyright   : Copyright(c) 2020, Rene Barto
//
// File        : ISoundAPI.h
//
// Namespace   : midi
//
// Class       : -
//
// Description : 
//
//------------------------------------------------------------------------------

#pragma once

#include <iostream>
#include <memory>

namespace sound {

class ISoundAPI
{
public:
    virtual ~ISoundAPI() = default;

    virtual bool Initialize() = 0;
    virtual void Uninitialize() = 0;
    virtual bool IsInitialized() = 0;
};

using ISoundAPIPtr = std::unique_ptr<ISoundAPI>;
extern ISoundAPIPtr CreateAPI();

} // namespace sound
