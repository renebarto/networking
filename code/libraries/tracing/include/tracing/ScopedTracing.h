#pragma once

#include "tracing/Tracing.h"

namespace tracing {

using ExitFunction = std::function<std::string()>;

class ScopedTracing
{
public:
    ScopedTracing(const std::string & fileName, int line, const std::string & functionName, const std::string & msg, ExitFunction exitFunction);
    ~ScopedTracing() noexcept;

private:
    std::string m_fileName;
    int m_line;
    std::string m_functionName;
    ExitFunction m_exitFunction;
};

#define SCOPEDTRACE(msg, exitFunction) tracing::ScopedTracing(__FILE__, __LINE__, __func__, msg, exitFunction)

} // namespace core
