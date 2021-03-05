//------------------------------------------------------------------------------
// Copyright   : Copyright(c) 2020, Rene Barto
//
// File        : RenderThread.cpp
//
// Namespace   : sound
//
// Class       : -
//
// Description : 
//
//------------------------------------------------------------------------------

#if defined(PLATFORM_WINDOWS)

#include "tracing/ScopedTracing.h"
#include "tracing/Logging.h"

#include "RenderThread.h"

namespace sound {

RenderThread::RenderThread()
    : core::ActiveObject("RenderThread")
{
    SCOPEDTRACE(
        nullptr, 
        nullptr);
}

RenderThread::~RenderThread()
{
    SCOPEDTRACE(
        nullptr, 
        nullptr);
}

void InitThread()
{
    SCOPEDTRACE(
        nullptr, 
        nullptr);
}

void Run()
{
    SCOPEDTRACE(
        nullptr, 
        nullptr);
}

void ExitThread()
{
    SCOPEDTRACE(
        nullptr, 
        nullptr);
}

void FlushThread()
{
    SCOPEDTRACE(
        nullptr, 
        nullptr);
}


} // namespace sound

#endif
