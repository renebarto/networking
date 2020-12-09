#include "tracing/ScopedTracing.h"

namespace tracing {

ScopedTracing::ScopedTracing(const std::string & fileName, int line, const std::string & functionName, const std::string & msg, ExitFunction exitFunction)
    : m_fileName(fileName)
    , m_line(line)
    , m_functionName(functionName)
    , m_exitFunction(exitFunction)
{
    Tracing::Trace(TraceCategory::FunctionBegin, fileName, line, functionName, msg);
}

ScopedTracing::~ScopedTracing()
{
    Tracing::Trace(TraceCategory::FunctionEnd, m_fileName, m_line, m_functionName, m_exitFunction ? m_exitFunction() : "");
}


} // namespace tracing