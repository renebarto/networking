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

class ISoundSource;

class ISoundAPI
{
public:
    virtual ~ISoundAPI() = default;

    virtual bool Initialize(const std::string & deviceName) = 0;
    virtual void Uninitialize() = 0;
    virtual bool IsInitialized() = 0;

    virtual bool Start(ISoundSource * audioSource) = 0;
    virtual void Stop() = 0;
};

using ISoundAPIPtr = std::unique_ptr<ISoundAPI>;
extern ISoundAPIPtr CreateAPI();

} // namespace sound
