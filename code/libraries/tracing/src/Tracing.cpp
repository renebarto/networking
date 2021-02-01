#include "tracing/Tracing.h"

#include <iomanip>
#include "osal/Console.h"
#include "osal/Thread.h"
#include "utility/Error.h"
#include "utility/GenericError.h"
#include "tracing/TraceHelpers.h"

namespace tracing {

TraceFunction Tracing::m_traceFunc = nullptr;
IsTraceCategoryEnabledFunction Tracing::m_isTraceCategoryEnabledFunc = nullptr;
CategorySet<TraceCategory> Tracing::m_defaultTraceFilter = TraceCategory::Startup | TraceCategory::Shutdown;
Tracing::Mutex Tracing::m_traceMutex;

static osal::Console s_traceConsole;

Tracing::~Tracing() noexcept
{
}

void Tracing::SetTracingFunction(TraceFunction traceFunc)
{
    m_traceFunc = traceFunc;
}

void Tracing::SetTracingEnabledFunction(IsTraceCategoryEnabledFunction enabledFunc)
{
    m_isTraceCategoryEnabledFunc = enabledFunc;
}

void SetDefaultTraceFilter(const CategorySet<TraceCategory> & defaultFilter)
{
    Tracing::m_defaultTraceFilter = defaultFilter;
}

CategorySet<TraceCategory> GetDefaultTraceFilter()
{
    return Tracing::m_defaultTraceFilter;
}

bool Tracing::IsTraceCategoryEnabled(TraceCategory category)
{
    if (m_isTraceCategoryEnabledFunc != nullptr)
    {
        return m_isTraceCategoryEnabledFunc(category);
    }
    else
    {
        return m_defaultTraceFilter.is_set(category);
    }
}

static osal::ConsoleColor GetColorForCategory(TraceCategory category)
{
    switch (category)
    {
    case TraceCategory::FunctionEnter:  return osal::ConsoleColor::Yellow;
    case TraceCategory::FunctionLeave:  return osal::ConsoleColor::Yellow;
    case TraceCategory::Startup:        return osal::ConsoleColor::Green;
    case TraceCategory::Shutdown:       return osal::ConsoleColor::Green;
    case TraceCategory::Log:            return osal::ConsoleColor::Magenta;
    case TraceCategory::Message:        return osal::ConsoleColor::White;
    case TraceCategory::Data:           return osal::ConsoleColor::Cyan;
    case TraceCategory::Debug:          return osal::ConsoleColor::Intensity | osal::ConsoleColor::Blue;

        default: return osal::ConsoleColor::Default;
    }
}

void Tracing::Trace(TraceCategory category, const std::string & path, int line, const std::string & functionName, const std::string & msg)
{
    if (!IsTraceCategoryEnabled(category))
        return;
    Lock guard(m_traceMutex);
    std::string fileName = ExtractFileName(path);
    auto clock = osal::Clock();
    if (m_traceFunc != nullptr)
    {
        m_traceFunc(clock.CurrentTime(), category, fileName, line, functionName, msg);
    }
    else
    {
        s_traceConsole << fgcolor(GetColorForCategory(category));
        s_traceConsole << clock << "|" << category << "|" << fileName << ":" << line << "|" << functionName << "|" << osal::GetThreadNameSelf() << "|" << msg << std::endl;
        s_traceConsole << fgcolor(osal::ConsoleColor::Default);
    }
}

} // namespace tracing
