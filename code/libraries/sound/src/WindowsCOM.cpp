//------------------------------------------------------------------------------
// Copyright   : Copyright(c) 2020, Rene Barto
//
// File        : WindowsCOM.cpp
//
// Namespace   : -
//
// Class       : -
//
// Description : 
//
//------------------------------------------------------------------------------

#if defined(PLATFORM_WINDOWS)

#include "tracing/ScopedTracing.h"

#include "WindowsCOM.h"

std::string GetErrorString(int errorCode)
{
    if (errorCode == 0) {
        return std::string(); //No error message has been recorded
    }
    
    LPSTR messageBuffer = nullptr;

    //Ask Win32 to give us the string version of that message ID.
    //The parameters we pass in, tell Win32 to create the buffer that holds the message for us (because we don't yet know how long the message string will be).
    size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
                                 nullptr, static_cast<DWORD>(errorCode), MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, nullptr);
    
    //Copy the error message into a std::string.
    std::string message(messageBuffer, size);
    
    //Free the Win32's string's buffer.
    LocalFree(messageBuffer);
            
    return message;
}

WindowsCOM::WindowsCOM()
    : m_isInitialized()
{
    SCOPEDTRACE(
        nullptr, 
        nullptr);
}

WindowsCOM::~WindowsCOM()
{
    SCOPEDTRACE(
        nullptr, 
        nullptr);

    Uninitialize();
}

bool WindowsCOM::Initialize()
{
    bool result = true;
    SCOPEDTRACE(
        nullptr, 
        [&] () { return utility::FormatString("result={}", result); });
    HRESULT hr = ::CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(hr))
    {
        LogError(__FILE__, __LINE__, __func__, "Unable to initialize COM: {,X:8}", hr);
        result = false;
    }
    if (result)
        m_isInitialized = true;
    return result;
}

void WindowsCOM::Uninitialize()
{
    SCOPEDTRACE(
        nullptr, 
        nullptr);

    if (!m_isInitialized)
        return;

    ::CoUninitialize();
    m_isInitialized = false;
}

#endif
