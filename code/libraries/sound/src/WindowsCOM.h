//------------------------------------------------------------------------------
// Copyright   : Copyright(c) 2020, Rene Barto
//
// File        : WindowsCOM.h
//
// Namespace   : -
//
// Class       : -
//
// Description : 
//
//------------------------------------------------------------------------------

#pragma once

#if defined(PLATFORM_WINDOWS)

#include <string>
#include <windows.h>

#include "tracing/ScopedTracing.h"

template <class T> void SafeRelease(T **ppT)
{
    if (*ppT)
    {
        (*ppT)->Release();
        *ppT = nullptr;
    }
}

template<class Interface>
class InterfaceWrapper
{
private:
    Interface * m_interface;

public:
    InterfaceWrapper(Interface * intf)
        : m_interface(intf)
    {
        SCOPEDTRACE(
            nullptr, 
            nullptr);
    }
    ~InterfaceWrapper()
    {
        SCOPEDTRACE(
            nullptr, 
            nullptr);
        SafeRelease(&m_interface);
    }

    Interface * Get() { return m_interface; }
};

class WindowsCOM
{
private:
    bool m_isInitialized;

public:
    WindowsCOM();
    ~WindowsCOM();
    bool Initialize();
    void Uninitialize();
};

std::string GetErrorString(int errorCode);

#endif
