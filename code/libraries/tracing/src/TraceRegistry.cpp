#include "tracing/TraceRegistry.h"
#include "tracing/TraceEntry.h"

namespace tracing {

TraceRegistry::TraceRegistry()
    : m_defaultTraceFilter()
    , m_traceEntries()
{

}

void TraceRegistry::SetDefaultTraceFilter(const CategorySet<TraceCategory> & traceFilter)
{
    m_defaultTraceFilter = traceFilter;
}

void TraceRegistry::SetTraceFilter(const std::string & compilationUnitName, const CategorySet<TraceCategory> & traceFilter)
{
    auto entry = FindCompilationUnit(compilationUnitName);
    if (entry != nullptr)
        entry->SetTraceFilter(traceFilter);
}

bool TraceRegistry::IsTraceCategoryEnabled(const std::string & compilationUnitName, TraceCategory category) const
{
    auto entry = FindCompilationUnit(compilationUnitName);
    if (entry != nullptr)
        return entry->IsTraceCategoryEnabled(category);
    return m_defaultTraceFilter.is_set(category);
}

void TraceRegistry::AddTraceEntry(TraceEntry * entry)
{
    auto it = m_traceEntries.find(entry->CompilationUnitFileName());
    if (it == m_traceEntries.end())
    {
        m_traceEntries.insert(std::make_pair(entry->CompilationUnitFileName(), entry));
    }
    else
    {
        it->second = entry;
    }
}

TraceEntry * TraceRegistry::FindCompilationUnit(const std::string & compilationUnitName) const
{
    auto it = m_traceEntries.find(compilationUnitName);
    if (it != m_traceEntries.end())
    {
        return it->second;
    }
    return nullptr;
}


} // namespace tracing

tracing::TraceRegistry & TraceRegistry()
{
    static tracing::TraceRegistry theRegistry;
    return theRegistry;
}

DEFINE_TRACE_ENTRY(__BASE_FILE__);