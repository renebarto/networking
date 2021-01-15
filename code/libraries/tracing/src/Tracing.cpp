#include "tracing/Tracing.h"

#include <iomanip>
#include "osal/Console.h"
#include "utility/Error.h"
#include "utility/GenericError.h"
#include "tracing/TraceHelpers.h"

namespace tracing {

TraceFunction Tracing::m_traceFunc = nullptr;
IsTraceCategoryEnabledFunction Tracing::m_isTraceCategoryEnabledFunc = nullptr;
Tracing::Mutex Tracing::m_traceMutex;

static osal::Console s_traceConsole;

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

        default: return osal::ConsoleColor::Default;
    }
}

void Tracing::Trace(TraceCategory category, const std::string & path, int line, const std::string & functionName, const std::string & msg)
{
    if (!IsTraceCategoryEnabled(category))
        return;
    Lock guard(m_traceMutex);
    std::string fileName = ExtractFileName(path);
    if (m_traceFunc != nullptr)
    {
        m_traceFunc(category, fileName, line, functionName, msg);
    }
    else
    {
        s_traceConsole << fgcolor(GetColorForCategory(category));
        s_traceConsole << category << "|" << fileName << ":" << line << "|" << functionName << "|" << msg << std::endl;
        s_traceConsole << fgcolor(osal::ConsoleColor::Default);
    }
}

} // namespace tracing
