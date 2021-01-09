#include "tracing/ScopedTracing.h"

namespace tracing {

ScopedTracing::ScopedTracing(const std::string & path, int line, const std::string & functionName, EntryFunction entryFunction, ExitFunction exitFunction)
    : m_path(path)
    , m_line(line)
    , m_functionName(functionName)
    , m_exitFunction(exitFunction)
{
    if (Tracing::IsTraceCategoryEnabled(TraceCategory::FunctionBegin) && Tracing::IsTraceCategoryEnabled(TraceCategory::FunctionEnd))
        Tracing::Trace(TraceCategory::FunctionBegin, path, line, functionName, entryFunction ? entryFunction() : "");
}

ScopedTracing::~ScopedTracing()
{
    if (Tracing::IsTraceCategoryEnabled(TraceCategory::FunctionBegin) && Tracing::IsTraceCategoryEnabled(TraceCategory::FunctionEnd))
        Tracing::Trace(TraceCategory::FunctionEnd, m_path, m_line, m_functionName, m_exitFunction ? m_exitFunction() : "");
}


} // namespace tracing