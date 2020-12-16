//------------------------------------------------------------------------------
// Copyright   : Copyright(c) 2009 Koninklijke Philips Electronics N.V.
//
// File        : Console.cpp
//
// Namespace   : core::utility
//
// Class       : Console
//
// Description :
//
//------------------------------------------------------------------------------

#include "utility/Console.h"

#include <string>

#if defined(PLATFORM_WINDOWS)
#include <io.h>
#endif

namespace utility {

#if defined(PLATFORM_WINDOWS)

#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2

FileDescriptor GetFileDescriptor(FILE * file)
{
    return ::_fileno(file);
}
const char * GetEnvironment(const char * name)
{
    static char buffer[4096];
    if (::GetEnvironmentVariableA(name, buffer, sizeof(buffer)) == 0)
    {
        return nullptr;
    }
    return buffer;
}
bool IsTTY(int fd)
{
    return ::_isatty(fd) != 0;
}

#else

FileDescriptor GetFileDescriptor(FILE * file)
{
    return ::fileno(file);
}
const char * GetEnvironment(const char * name)
{
    return ::getenv(name);
}
bool IsTTY(int fd)
{
    return ::isatty(fd) != 0;
}

#endif

static std::ostream * DetermineStream(FileDescriptor handle)
{
    switch (handle)
    {
    case STDOUT_FILENO:
        return &std::cout;
    case STDERR_FILENO:
        return &std::cerr;
    case STDIN_FILENO:
    default:
        std::cerr << "Invalid handle specified, please specify only stdout or stderr handle"
                  << std::endl;
    }
    return nullptr;
}

static FileDescriptor DetermineHandle(std::ostream * stream)
{
    if (stream == &std::cout)
        return GetFileDescriptor(stdout);
    else if (stream == &std::cerr)
        return GetFileDescriptor(stderr);
    return InvalidHandle;
}

static const char * GetAnsiColorCode(ConsoleColor color)
{
    switch (color & ConsoleColor::ColorMask)
    {
    case ConsoleColor::Black:
        return "0";
    case ConsoleColor::Red:
        return "1";
    case ConsoleColor::Green:
        return "2";
    case ConsoleColor::Yellow:
        return "3";
    case ConsoleColor::Blue:
        return "4";
    case ConsoleColor::Magenta:
        return "5";
    case ConsoleColor::Cyan:
        return "6";
    case ConsoleColor::White:
        return "7";
    default:
        return nullptr;
    };
}

Console::Console(int handle)
    : m_handle(handle)
    , m_stream(DetermineStream(m_handle))
    , m_forceUseColor()
    , m_currentForegroundColor(ConsoleColor::Default)
    , m_currentBackgroundColor(ConsoleColor::Default)
{
}

Console::Console(FILE * file)
    : m_handle(GetFileDescriptor(file))
    , m_stream(DetermineStream(m_handle))
    , m_forceUseColor()
    , m_currentForegroundColor(ConsoleColor::Default)
    , m_currentBackgroundColor(ConsoleColor::Default)
{
}

Console::Console(std::ostream & stream)
    : m_handle(DetermineHandle(&stream))
    , m_stream(&stream)
    , m_forceUseColor()
    , m_currentForegroundColor(ConsoleColor::Default)
    , m_currentBackgroundColor(ConsoleColor::Default)
{
}

void Console::SetTerminalColor(ConsoleColor foregroundColor, ConsoleColor backgroundColor)
{
    if (!ShouldUseColor())
        return;
    std::string command = "\033[0";
    if (foregroundColor != ConsoleColor::Default)
    {
        if ((foregroundColor & ConsoleColor::Intensity) == ConsoleColor::Intensity)
            command += ";9";
        else
            command += ";3";
        command += GetAnsiColorCode(foregroundColor);
        if ((foregroundColor & ConsoleColor::Bold) == ConsoleColor::Bold)
        {
            command += ";1";
        }
    }
    if (backgroundColor != ConsoleColor::Default)
    {
        if ((backgroundColor & ConsoleColor::Intensity) == ConsoleColor::Intensity)
            command += ";10";
        else
            command += ";4";
        command += GetAnsiColorCode(backgroundColor);
        if ((backgroundColor & ConsoleColor::Bold) == ConsoleColor::Bold)
        {
            command += ";1";
        }
    }
    command += "m";
    if (m_stream)
        *m_stream << command;
    m_currentForegroundColor = foregroundColor;
    m_currentBackgroundColor = backgroundColor;
}

bool Console::ShouldUseColor() const
{
    if (ForceUseColor())
        return true;
    if (m_handle == InvalidHandle)
        return false;
    if (!IsTTY(m_handle))
        return false;

    const char * termSetting = GetEnvironment("TERM");
    if (!termSetting)
        return false;
    std::string term = termSetting;
    const bool terminalSupportsColor = (term == "xterm") || (term == "xterm-color")
        || (term == "xterm-256color") || (term == "screen") || (term == "screen-256color")
        || (term == "linux") || (term == "cygwin");
    return terminalSupportsColor;
}

bool Console::ForceUseColor() const
{
    return m_forceUseColor;
}

void Console::ForceUseColor(bool value)
{
    m_forceUseColor = value;
}

} // namespace utility

utility::_SetForegroundColor fgcolor(utility::ConsoleColor color)
{
    return {color};
}

utility::_SetBackgroundColor bgcolor(utility::ConsoleColor color)
{
    return {color};
}
