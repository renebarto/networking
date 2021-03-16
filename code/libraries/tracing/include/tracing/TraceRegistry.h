#pragma once

#include <map>
#include <string>
#include "tracing/CategorySet.h"
#include "tracing/TraceCategory.h"

namespace tracing {

class TraceEntry;

class TraceRegistry
{
private:
    CategorySet<TraceCategory> m_defaultTraceFilter;
    std::map<std::string, TraceEntry *> m_traceEntries;

public:
    TraceRegistry();

    void SetDefaultTraceFilter(const CategorySet<TraceCategory> & traceFilter);
    void SetTraceFilter(const std::string & compilationUnitName, const CategorySet<TraceCategory> & traceFilter);
    bool IsTraceCategoryEnabled(const std::string & compilationUnitName, TraceCategory category) const;

    void AddTraceEntry(TraceEntry * entry);
    TraceEntry * FindCompilationUnit(const std::string & compilationUnitName) const;
};

} // namespace tracing

tracing::TraceRegistry & TraceRegistry();