#include "tracing/Tracing.h"

#include <iomanip>
#include "osal/Console.h"
#include "osal/ThreadFunctions.h"
#include "utility/Error.h"
#include "utility/GenericError.h"
#include "tracing/TraceEntry.h"
#include "tracing/TraceHelpers.h"

namespace tracing {

TraceFunction Tracing::m_traceFunc = nullptr;
IsTraceCategoryEnabledFunction Tracing::m_isTraceCategoryEnabledFunc = nullptr;
CategorySet<TraceCategory> Tracing::m_defaultTraceFilter = 
    (TraceCategory::SscfBegin | TraceCategory::SscfEnd | TraceCategory::SscfEvent | 
     TraceCategory::SscfLib | TraceCategory::ControlLayer | TraceCategory::CanNmtDbt | 
     TraceCategory::Information | TraceCategory::Log | TraceCategory::BistPostInfo | 
     TraceCategory::StartupShutdown | TraceCategory::ResultFlow);
Tracing::Mutex Tracing::m_traceMutex;
FatalExitFunction Tracing::m_fatalExitFunc = nullptr;

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

void Tracing::SetFatalExitFunction(FatalExitFunction function)
{
    Tracing::m_fatalExitFunc = function;
}

void Tracing::FatalExit(int errorCode)
{
    if (m_fatalExitFunc != nullptr)
    {
        return m_fatalExitFunc(errorCode);
    }
    else
    {
        exit(errorCode);
    }
}

static osal::ConsoleColor GetColorForCategory(TraceCategory category)
{
    switch (category)
    {
    case TraceCategory::FunctionEnter:  return osal::ConsoleColor::Yellow;
    case TraceCategory::FunctionLeave:  return osal::ConsoleColor::Yellow;
    case TraceCategory::StartupShutdown:return osal::ConsoleColor::Green;
    case TraceCategory::Log:            return osal::ConsoleColor::Magenta;
    case TraceCategory::Information:    return osal::ConsoleColor::White;
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

void Tracing::Log(const std::string & path, int line, const std::string & functionName, const std::string & msg)
{
    if (IsTraceCategoryEnabled(TraceCategory::Log))
    {
        Trace(TraceCategory::Log, path, line, functionName, msg);
    }
}

void Tracing::Error(const std::string & path, int line, const std::string & functionName, const utility::Error & error)
{
    auto errorCode = error.ErrorCode();
    if (errorCode != -1)
    {
        Log(path, line, functionName, "Error code: {}", utility::Serialize(error));
    }
    else
    {
        Log(path, line, functionName, "Error code: Unknown: {}", error.Message());
    }
}

void Tracing::Error(const std::string & path, int line, const std::string & functionName, const utility::GenericError & error)
{
    Log(path, line, functionName, utility::Serialize(error));
}

void Tracing::Fatal(const std::string & path, int line, const std::string & functionName, const utility::Error & error)
{
    auto errorCode = error.ErrorCode();
    if (errorCode != -1)
    {
        Log(path, line, functionName, "Error code: {}", utility::Serialize(error));
    }
    else
    {
        Log(path, line, functionName, "Error code: Unknown: {}", error.Message());
    }
    FatalExit(error.ErrorCode());
}

void Tracing::Fatal(const std::string & path, int line, const std::string & functionName, const utility::GenericError & error)
{
    Log(path, line, functionName, utility::Serialize(error));
    FatalExit(1);
}

void Tracing::Throw(const std::string & path, int line, const std::string & functionName, const utility::Error & error)
{
    std::ostringstream stream;
    stream 
        << osal::Clock() << "|" << TraceCategory::Log << " " << ExtractFileName(path) << ":" << line << " (" << functionName << "): " << error.Message() << ": Error code: "
        << std::dec << error.ErrorCode() << " (" << std::hex << std::setw(2) << std::setfill('0') << error.ErrorCode() << "): "
        << error.ErrorString();
    throw std::runtime_error(stream.str());
}

void Tracing::Throw(const std::string & path, int line, const std::string & functionName, const utility::GenericError & error)
{
    std::ostringstream stream;
    stream << osal::Clock() << "|" << TraceCategory::Log << " " << ExtractFileName(path) << ":" << line << " (" << functionName << "): " << error;
    throw std::runtime_error(stream.str());
}

} // namespace tracing
