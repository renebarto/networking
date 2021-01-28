//------------------------------------------------------------------------
// Copyright   : Copyright(c) 2020, Koninklijke Philips Electronics N.V.
//
// File        : ConsoleTest.cpp
//
// Namespace   : core::utility
//
// Class       : -
//
// Description :
//  Console tests
//
//---------------------------------------------------------------

#include "GoogleTest.h"

#include "osal/Console.h"

namespace osal {

TEST(ConsoleTest, Constructor)
{
    std::ostringstream stream;
    Console console(stream);
    EXPECT_FALSE(console.ShouldUseColor());
    EXPECT_FALSE(console.ForceUseColor());
}

TEST(ConsoleTest, ConstructorStdOut)
{
    const char * termVar = GetEnvironment("TERM");
    std::string term = termVar ? termVar : "";
    bool terminalSupportsColor {};
    Console console(std::cout);
#if defined(PLATFORM_WINDOWS)
    terminalSupportsColor = true;
#else
    terminalSupportsColor = console.IsTTY() &&
        ((term == "xterm") || (term == "xterm-color") || (term == "xterm-256color") || 
         (term == "screen") || (term == "screen-256color") || (term == "linux") || (term == "cygwin"));
#endif
    EXPECT_EQ(terminalSupportsColor, console.ShouldUseColor());
    EXPECT_FALSE(console.ForceUseColor());
}

TEST(ConsoleTest, ForceUseColor)
{
    std::ostringstream stream;
    Console console(stream);
    console.ForceUseColor(true);
    EXPECT_TRUE(console.ShouldUseColor());
    EXPECT_TRUE(console.ForceUseColor());
}

TEST(ConsoleTest, OutputNoColor)
{
    std::ostringstream stream;
    Console console(stream);
    console << "no color\n"
            << fgcolor(ConsoleColor::Red) << "red\n"
            << fgcolor(ConsoleColor::Green | ConsoleColor::Intensity)
            << bgcolor(ConsoleColor::White | ConsoleColor::Intensity) << "green on black\n"
            << fgcolor(ConsoleColor::Default) << bgcolor(ConsoleColor::Default) << "no color\n";
    std::string expected("no color\nred\ngreen on black\nno color\n");
    EXPECT_EQ(expected, stream.str());
}

TEST(ConsoleTest, OutputColor)
{
    std::ostringstream stream;
    Console console(stream);
    console.ForceUseColor(true);
    console << "no color\n"
            << fgcolor(ConsoleColor::Red) << "red\n"
            << fgcolor(ConsoleColor::Green | ConsoleColor::Intensity)
            << bgcolor(ConsoleColor::White | ConsoleColor::Intensity) << "green on black\n"
            << fgcolor(ConsoleColor::Default) << bgcolor(ConsoleColor::Default) << "no color\n";
    std::string expected("no color\n\033[0;31mred\n\033[0;92m\033[0;92;107mgreen on black\n\033[0;107m\033[0mno color\n");
    EXPECT_EQ(expected, stream.str());
}

TEST(ConsoleTest, SetForegroundColorNoColor)
{
    std::ostringstream stream;
    Console console(stream);
    console.SetForegroundColor(ConsoleColor::Blue | ConsoleColor::Bold);
    console << "text";
    console.SetForegroundColor(ConsoleColor::Default);
    console << "text";
    std::string expected("texttext");
    EXPECT_EQ(expected, stream.str());
}

TEST(ConsoleTest, SetForegroundColorWithColor)
{
    std::ostringstream stream;
    Console console(stream);
    console.ForceUseColor(true);
    console.SetForegroundColor(ConsoleColor::Blue | ConsoleColor::Bold);
    console << "text";
    console.SetForegroundColor(ConsoleColor::Default);
    console << "text";
    std::string expected("\033[0;34;1mtext\033[0mtext");
    EXPECT_EQ(expected, stream.str());
}

TEST(ConsoleTest, SetBackgroundColorNoColor)
{
    std::ostringstream stream;
    Console console(stream);
    console.SetBackgroundColor(ConsoleColor::Blue | ConsoleColor::Bold);
    console << "text";
    console.SetBackgroundColor(ConsoleColor::Default);
    console << "text";
    std::string expected("texttext");
    EXPECT_EQ(expected, stream.str());
}

TEST(ConsoleTest, SetBackgroundColorWithColor)
{
    std::ostringstream stream;
    Console console(stream);
    console.ForceUseColor(true);
    console.SetBackgroundColor(ConsoleColor::Blue | ConsoleColor::Bold);
    console << "text";
    console.SetBackgroundColor(ConsoleColor::Default);
    console << "text";
    std::string expected("\033[0;44;1mtext\033[0mtext");
    EXPECT_EQ(expected, stream.str());
}

TEST(ConsoleTest, SetResetTerminalColorNoColor)
{
    std::ostringstream stream;
    Console console(stream);
    console.SetTerminalColor(ConsoleColor::Blue | ConsoleColor::Bold);
    console << "text";
    console.ResetTerminalColor();
    console << "text";
    std::string expected("texttext");
    EXPECT_EQ(expected, stream.str());
}

TEST(ConsoleTest, SetResetTerminalColorWithColor)
{
    std::ostringstream stream;
    Console console(stream);
    console.ForceUseColor(true);
    console.SetTerminalColor(ConsoleColor::Blue | ConsoleColor::Bold, ConsoleColor::Red | ConsoleColor::Intensity);
    console << "text";
    console.ResetTerminalColor();
    console << "text";
    std::string expected("\033[0;34;1;101mtext\033[0mtext");
    EXPECT_EQ(expected, stream.str());
}

} // namespace osal
