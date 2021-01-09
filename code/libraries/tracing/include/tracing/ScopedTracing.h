#pragma once

#include "tracing/Tracing.h"

namespace tracing {

using EntryFunction = std::function<std::string()>;
using ExitFunction = std::function<std::string()>;

class ScopedTracing
{
public:
    ScopedTracing(const std::string & path, int line, const std::string & functionName, EntryFunction entryFunction, ExitFunction exitFunction);
    ~ScopedTracing() noexcept;

private:
    std::string m_path;
    int m_line;
    std::string m_functionName;
    ExitFunction m_exitFunction;
};

#define TOKENPASTE(x, y, z) x ## y ## z
#define TOKENPASTE3(x, y, z) TOKENPASTE(x, y, z)
#define SCOPEDTRACE(entryFunction, exitFunction) tracing::ScopedTracing TOKENPASTE3(_trace, __func__, __LINE__)(__FILE__, __LINE__, __func__, entryFunction, exitFunction)

} // namespace tracing
