#pragma once

#include <functional>

namespace utility
{
    using AssertHandler = std::function<void (bool expression, const char * expressionText, const char * file, int line, const char * func)>;

    extern void InternalAssertionHandler(bool expression, const char * expressionText, const char * file, int line, const char * func);
    extern AssertHandler SetAssertionHandler(AssertHandler handler);

} // namespace utility

#define ASSERT(expression) \
utility::InternalAssertionHandler(expression, #expression, __FILE__, __LINE__, __func__)
