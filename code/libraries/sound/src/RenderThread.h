//------------------------------------------------------------------------------
// Copyright   : Copyright(c) 2020, Rene Barto
//
// File        : SoundClient.h
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

#include "core/ActiveObject.h"

namespace sound {

class RenderThread
    : public core::ActiveObject
{
public:
    RenderThread();
    ~RenderThread();
    
    void InitThread() override;
    void Run() override;
    void ExitThread() override;
    void FlushThread() override;
};

} // namespace sound

#endif