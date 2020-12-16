#include "tracing/Tracing.h"

#include <iostream>
#include <iomanip>
#include "utility/Console.h"
#include "utility/EnumSerialization.h"
#include "utility/Error.h"
#include "utility/GenericError.h"

namespace tracing {

TraceFunction Tracing::m_traceFunc = nullptr;
IsTraceCategoryEnabledFunction Tracing::m_isTraceCategoryEnabledFunc = nullptr;

static utility::Console s_traceConsole;

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

utility::ConsoleColor GetColorForCategory(TraceCategory category)
{
    switch (category)
    {
        case TraceCategory::FunctionBegin:  return utility::ConsoleColor::Yellow;
        case TraceCategory::FunctionEnd:    return utility::ConsoleColor::Yellow;
        case TraceCategory::Information:    return utility::ConsoleColor::Cyan;
        case TraceCategory::Log:            return utility::ConsoleColor::Magenta;
        case TraceCategory::Startup:        return utility::ConsoleColor::Green;
        case TraceCategory::Shutdown:       return utility::ConsoleColor::Green;
        case TraceCategory::Debug:          return utility::ConsoleColor::White;
        case TraceCategory::Error:          return utility::ConsoleColor::Red;

        default: return utility::ConsoleColor::Default;
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
        s_traceConsole << fgcolor(GetColorForCategory(category));
        s_traceConsole << "Category: " << category << " " << fileName << ":" << line << " - " << functionName << ":" << msg << std::endl;
        s_traceConsole << fgcolor(utility::ConsoleColor::Default);
    }
}

void Tracer::Trace(const std::string & fileName, int line , const std::string & functionName, const utility::Error & error)
{
    auto errorCode = error.ErrorCode();
    if (errorCode != -1)
    {
        Tracing::Trace(TraceCategory::Error, fileName, line, functionName, "Error code: {}", utility::Serialize(error));
    }
    else
    {
        Tracing::Trace(TraceCategory::Error, fileName, line, functionName, "Error code: Unknown: {}", error.Message());
    }
}

void Tracer::Fatal(const std::string & fileName, int line , const std::string & functionName, const utility::Error & error)
{
    Trace(fileName, line, functionName, error);
    exit(1);
}

void Tracer::Throw(const std::string & fileName, int line , const std::string & functionName, const utility::Error & error)
{
    std::ostringstream stream;
    stream << TraceCategory::Error << " " << fileName << ":" << line << "(" << functionName << "): Error code: " << error;
    throw std::runtime_error(stream.str());
}

void Tracer::Throw(const std::string & fileName, int line , const std::string & functionName, const utility::GenericError & error)
{
    std::ostringstream stream;
    stream << TraceCategory::Error << " " << fileName << ":" << line << "(" << functionName << "): " << error;
    throw std::runtime_error(stream.str());
}

std::ostream & operator << (std::ostream & stream, const TraceCategory & value)
{
    return stream << utility::Serialize(value, "????");
}

} // namespace tracing

namespace utility {

template<>
const BidirectionalMap<tracing::TraceCategory, std::string> EnumSerializationMap<tracing::TraceCategory>::ConversionMap = {
    { tracing::TraceCategory::FunctionBegin, "FuncBeg"},
    { tracing::TraceCategory::FunctionEnd, "FuncEnd"},
    { tracing::TraceCategory::Information, "Info"},
    { tracing::TraceCategory::Log, "Log"},
    { tracing::TraceCategory::Startup, "Startup"},
    { tracing::TraceCategory::Shutdown, "Shutdown"},
    { tracing::TraceCategory::Debug, "Debug"},
    { tracing::TraceCategory::Error, "Error"}
};

} // namespace utility