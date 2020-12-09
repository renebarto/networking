#include "utility/Assert.h"

#include <string>
#include <cassert>
#if defined(PLATFORM_WINDOWS)
#define NTDDI_WIN7SP1                       0x0601010
#if _MSC_VER > 1900 // Versions after VS 2015
#pragma warning(disable: 5039)
#endif
#include <windows.h>
#if _MSC_VER > 1900 // Versions after VS 2015
#pragma warning(default: 5039)
#endif
#endif

namespace utility {

static AssertHandler currentAssertHandler = nullptr;

void InternalAssertionHandler(bool expression, const char * expressionText, const char * file, int line, const char * func)
{
    if (!expression)
    {
        if (currentAssertHandler != nullptr)
            currentAssertHandler(expression, expressionText, file, line, func);
        else
        {
#if defined(PLATFORM_WINDOWS)
            char lineNumberText[10];
            _itoa(line, lineNumberText, 10);
            std::string output = std::string(file) + ": " + lineNumberText + ": " + func + ": " + expressionText;
            OutputDebugStringA(output.c_str());
#elif defined(PLATFORM_LINUX)
#ifdef _DEBUG
            __assert_fail(expressionText, file, line, func);
#endif
#endif
        }
    }
}

AssertHandler SetAssertionHandler(AssertHandler handler)
{
    AssertHandler previousHandler = currentAssertHandler;
    currentAssertHandler = handler;
    return previousHandler;
}

} // namespace utility
