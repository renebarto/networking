//------------------------------------------------------------------------------
// Copyright   : Copyright(c) 2009 Koninklijke Philips Electronics N.V.
//
// File        : Console.h
//
// Namespace   : core::utility
//
// Class       : Console
//
// Description :
//  ANSI console class
//
//------------------------------------------------------------------------------

#pragma once

#include <iostream>
#include <mutex>

#include "osal/FileHandling.h"

namespace osal
{

    class Secret;

} // namespace osal

// Ensures that there is at least one operator<< in the global namespace.
// See Message& operator<<(...) below for why.
void operator <<(const osal::Secret&, int);

namespace osal {

enum class ConsoleColor : int {
    Default = -1,
    Black = 0,
    Red,
    Green,
    Yellow,
    Blue,
    Magenta,
    Cyan,
    White,
    ColorMask = 0x07,
    Intensity = 0x08,
    Bold = 0x10,
};

std::ostream & operator << (std::ostream & stream, ConsoleColor value);

struct _SetForegroundColor
{
    ConsoleColor color;
};

struct _SetBackgroundColor
{
    ConsoleColor color;
};

class Console
{
protected:
    // The type of basic IO manipulators (endl, ends, and flush) for narrow
    // streams.
    typedef std::ostream & (* BasicIoManip)(std::ostream &);

public:
    Console(int handle = GetFileDescriptor(stdout));
    Console(FILE * file);
    Console(std::ostream & stream);

    std::ostream * GetStream() { return m_stream; }

    void SetForegroundColor(ConsoleColor foregroundColor)
    {
        SetTerminalColor(foregroundColor, m_currentBackgroundColor);
    }
    void SetBackgroundColor(ConsoleColor backgroundColor)
    {
        SetTerminalColor(m_currentForegroundColor, backgroundColor);
    }
    void SetTerminalColor(ConsoleColor foregroundColor = ConsoleColor::Default,
                          ConsoleColor backgroundColor = ConsoleColor::Default);
    void ResetTerminalColor()
    {
        SetTerminalColor();
    }
    bool ShouldUseColor() const;
    bool ForceUseColor() const;
    void ForceUseColor(bool value);

    // Streams a non-pointer value to this object.
    template <typename T>
    Console & operator << (const T & val)
    {
        Lock lock(m_mutex);
        using ::operator <<;
        if (m_stream)
            *m_stream << val;
        return *this;
    }

    Console & operator << (BasicIoManip val)
    {
        Lock lock(m_mutex);
        if (m_stream)
            *m_stream << val;
        return *this;
    }

    Console & operator << (_SetForegroundColor color)
    {
        Lock lock(m_mutex);
        SetForegroundColor(color.color);
        return *this;
    }
    Console & operator << (_SetBackgroundColor color)
    {
        Lock lock(m_mutex);
        SetBackgroundColor(color.color);
        return *this;
    }

protected:
    using Mutex = std::recursive_mutex;
    using Lock = std::lock_guard<Mutex>;

    FileDescriptor m_handle;
    std::ostream * m_stream;
    Mutex m_mutex;
    bool m_forceUseColor;
    ConsoleColor m_currentForegroundColor;
    ConsoleColor m_currentBackgroundColor;
};

inline ConsoleColor operator &(ConsoleColor lhs, ConsoleColor rhs)
{
    return static_cast<ConsoleColor>(static_cast<int>(lhs) & static_cast<int>(rhs));
}
inline ConsoleColor operator |(ConsoleColor lhs, ConsoleColor rhs)
{
    return static_cast<ConsoleColor>(static_cast<int>(lhs) | static_cast<int>(rhs));
}

} // namespace osal

osal::_SetForegroundColor fgcolor(osal::ConsoleColor color);
osal::_SetBackgroundColor bgcolor(osal::ConsoleColor color);

