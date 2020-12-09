#include "tracing/Tracing.h"

#include <iostream>
#include "utility/BidirectionalMap.h"

namespace tracing {

TraceFunction Tracing::m_traceFunc = nullptr;
IsTraceCategoryEnabledFunction Tracing::m_isTraceCategoryEnabledFunc = nullptr;

Tracing::~Tracing() noexcept
{
}

void Tracing::SetTracingFunctions(TraceFunction traceFunc, IsTraceCategoryEnabledFunction enabledFunc)
{
    m_traceFunc = traceFunc;
    m_isTraceCategoryEnabledFunc = enabledFunc;
}

bool Tracing::IsTraceCategoryEnabled(TraceCategory category)
{
    if (m_isTraceCategoryEnabledFunc != nullptr)
    {
        return m_isTraceCategoryEnabledFunc(category);
    }
    else
    {
        return false;
    }
}

void Tracing::Trace(TraceCategory category, const std::string & fileName, int line , const std::string & functionName, const std::string & msg)
{
    if (!IsTraceCategoryEnabled(category))
        return;
    if (m_traceFunc != nullptr)
    {
        m_traceFunc(category, fileName, line, functionName, msg);
    }
    else
    {
        std::cout << "Category: " << category << " " << fileName << ":" << line << " - " << functionName << ":" << msg << std::endl;
    }
}

static const utility::BidirectionalMap<TraceCategory, std::string> CategoryMap {
    { TraceCategory::FunctionBegin, "FuncBeg"},
    { TraceCategory::FunctionEnd, "FuncEnd"},
    { TraceCategory::Information, "Info"},
    { TraceCategory::Log, "Log"},
    { TraceCategory::Startup, "StartShut"},
    { TraceCategory::Shutdown, "StartShut"},
    { TraceCategory::Debug, "Dbg"}
};

std::ostream & operator << (std::ostream & stream, const TraceCategory & value)
{
    return stream << CategoryMap.Translate(value, "????");
}

} // namespace tracing