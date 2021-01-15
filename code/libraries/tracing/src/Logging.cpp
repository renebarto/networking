#include "tracing/Logging.h"

#include <iomanip>
#include "osal/Console.h"
#include "utility/Error.h"
#include "utility/GenericError.h"
#include "tracing/TraceHelpers.h"

namespace tracing {

LogFunction Logging::m_logFunc = nullptr;
IsLogCategoryEnabledFunction Logging::m_isLogCategoryEnabledFunc = nullptr;
FatalExitFunction Logging::m_fatalExitFunc = nullptr;
Logging::Mutex Logging::m_logMutex;

static osal::Console s_logConsole;

Logging::~Logging() noexcept
{
}

void Logging::SetLoggingFunctions(LogFunction logFunc, IsLogCategoryEnabledFunction enabledFunc)
{
    m_logFunc = logFunc;
    m_isLogCategoryEnabledFunc = enabledFunc;
}

bool Logging::IsLogCategoryEnabled(LogCategory category)
{
    if (m_isLogCategoryEnabledFunc != nullptr)
    {
        return m_isLogCategoryEnabledFunc(category);
    }
    else
    {
        return false;
    }
}

void Logging::SetFatalExitFunction(FatalExitFunction function)
{
    Logging::m_fatalExitFunc = function;
}

void Logging::FatalExit(int errorCode)
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

static osal::ConsoleColor GetColorForCategory(LogCategory category)
{
    switch (category)
    {
        case LogCategory::Trace:        return osal::ConsoleColor::Magenta;
        case LogCategory::Debug:        return osal::ConsoleColor::Magenta;
        case LogCategory::Information:  return osal::ConsoleColor::Cyan;
        case LogCategory::Warning:      return osal::ConsoleColor::Yellow;
        case LogCategory::Error:        return osal::ConsoleColor::Red;
        case LogCategory::Fatal:        return osal::ConsoleColor::Red | osal::ConsoleColor::Intensity;

        default: return osal::ConsoleColor::Default;
    }
}

void Logging::Log(LogCategory category, const std::string & path, int line, const std::string & functionName, const std::string & msg)
{
    if (!IsLogCategoryEnabled(category))
        return;
    Lock guard(m_logMutex);
    std::string fileName = ExtractFileName(path);
    if (m_logFunc != nullptr)
    {
        m_logFunc(category, fileName, line, functionName, msg);
    }
    else
    {
        s_logConsole << fgcolor(GetColorForCategory(category));
        s_logConsole << category << "|" << fileName << ":" << line << "|" << functionName << "|" << msg << std::endl;
        s_logConsole << fgcolor(osal::ConsoleColor::Default);
    }
}

void Logging::Error(const std::string & path, int line, const std::string & functionName, const utility::Error & error)
{
    auto errorCode = error.ErrorCode();
    if (errorCode != -1)
    {
        Log(LogCategory::Error, path, line, functionName, "Error code: {}", utility::Serialize(error));
    }
    else
    {
        Log(LogCategory::Error, path, line, functionName, "Error code: Unknown: {}", error.Message());
    }
}

void Logging::Error(const std::string & path, int line, const std::string & functionName, const utility::GenericError & error)
{
    Log(LogCategory::Error, path, line, functionName, utility::Serialize(error));
}

void Logging::Fatal(const std::string & path, int line, const std::string & functionName, const utility::Error & error)
{
    auto errorCode = error.ErrorCode();
    if (errorCode != -1)
    {
        Log(LogCategory::Fatal, path, line, functionName, "Error code: {}", utility::Serialize(error));
    }
    else
    {
        Log(LogCategory::Fatal, path, line, functionName, "Error code: Unknown: {}", error.Message());
    }
    FatalExit(error.ErrorCode());
}

void Logging::Fatal(const std::string & path, int line, const std::string & functionName, const utility::GenericError & error)
{
    Log(LogCategory::Fatal, path, line, functionName, utility::Serialize(error));
    FatalExit(1);
}

void Logging::Throw(const std::string & path, int line, const std::string & functionName, const utility::Error & error)
{
    std::ostringstream stream;
    stream 
        << LogCategory::Error << " " << ExtractFileName(path) << ":" << line << "(" << functionName << "): " << error.Message() << ": Error code: "
        << std::dec << error.ErrorCode() << " (" << std::hex << std::setw(2) << std::setfill('0') << error.ErrorCode() << "): "
        << error.ErrorString();
    throw std::runtime_error(stream.str());
}

void Logging::Throw(const std::string & path, int line, const std::string & functionName, const utility::GenericError & error)
{
    std::ostringstream stream;
    stream << LogCategory::Error << " " << ExtractFileName(path) << ":" << line << "(" << functionName << "): " << error;
    throw std::runtime_error(stream.str());
}

} // namespace tracing
