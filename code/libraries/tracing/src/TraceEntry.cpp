#include "tracing/TraceEntry.h"

namespace tracing {

TraceEntry::TraceEntry(const std::string & compilationUnitFileName, TraceRegistry & traceRegistry)
    : m_traceFilter()
    , m_compilationUnitFileName(RelativePath(compilationUnitFileName))
{
    traceRegistry.AddTraceEntry(this);
}

const std:: string & TraceEntry::CompilationUnitFileName()
{
    return m_compilationUnitFileName;
}

void TraceEntry::SetTraceFilter(const CategorySet<TraceCategory> & traceFilter)
{
    m_traceFilter = traceFilter;
}

bool TraceEntry::IsTraceCategoryEnabled(TraceCategory category) const
{
    return m_traceFilter.is_set(category);
}

} // namespace tracing

DEFINE_TRACE_ENTRY(__BASE_FILE__);