#pragma once

#include <functional>
#include <mutex>
#include <string>
#include "osal/Clock.h"
#include "utility/Format.h"
#include "tracing/CategorySet.h"
#include "tracing/LogCategory.h"

namespace utility {

class Error;
class GenericError;

}

namespace tracing {

using LogFunction = std::function<
    void (osal::EpochTime timestamp,
          LogCategory category,
          const std::string & fileName, 
          int line, 
          const std::string & functionName, 
          const std::string & msg)>;

using IsLogCategoryEnabledFunction = std::function<bool(LogCategory category)>;
using FatalExitFunction = std::function<void(int errorCode)>;

void SetDefaultLogFilter(const CategorySet<LogCategory> & defaultFilter);
CategorySet<LogCategory> GetDefaultLogFilter();

class Logging
{
private:
    friend void SetDefaultLogFilter(const CategorySet<LogCategory> & defaultFilter);
    friend CategorySet<LogCategory> GetDefaultLogFilter();

    static LogFunction m_logFunc;
    static IsLogCategoryEnabledFunction m_isLogCategoryEnabledFunc;
    static FatalExitFunction m_fatalExitFunc;
    static CategorySet<LogCategory> m_defaultLogFilter;

    typedef std::recursive_mutex Mutex;
    typedef std::lock_guard<Mutex> Lock;
    static Mutex m_logMutex;

    std::string m_fileName;
    std::string m_functionName;

public:
    Logging() = default;
    virtual ~Logging() noexcept;
    
    static void SetLoggingFunction(LogFunction logFunc);
    static void SetLoggingEnabledFunction(IsLogCategoryEnabledFunction enabledFunc);
    static void SetFatalExitFunction(FatalExitFunction function);
    
    static bool IsLogCategoryEnabled(LogCategory category);
    static void FatalExit(int errorCode);
    static void Log(LogCategory category, const std::string & path, int line, const std::string & functionName, const std::string & message);
    template <typename ... Args>
    static void Log(LogCategory category, const std::string & path, int line, const std::string & functionName, const std::string & format, Args const & ... args) noexcept
    {
        if (IsLogCategoryEnabled(category))
        {
            std::string str;
            utility::Format(str, format, args ...);
            Log(category, path, line, functionName, str);
        }
    }
    static void Error(const std::string & path, int line, const std::string & functionName, const utility::Error & error);
    static void Error(const std::string & path, int line, const std::string & functionName, const utility::GenericError & error);
    static void Fatal(const std::string & path, int line, const std::string & functionName, const utility::Error & error);
    static void Fatal(const std::string & path, int line, const std::string & functionName, const utility::GenericError & error);
    static void Throw(const std::string & path, int line, const std::string & functionName, const utility::Error & error);
    static void Throw(const std::string & path, int line, const std::string & functionName, const utility::GenericError & error);
};

std::ostream & operator << (std::ostream & stream, const LogCategory & value);

} // namespace tracing

inline void LogDebug(const std::string & path, int line, const std::string & functionName, const std::string & message)
{
    tracing::Logging::Log(tracing::LogCategory::Debug, path, line, functionName, message);
}
template <typename ... Args>
void LogDebug(const std::string & path, int line, const std::string & functionName, const std::string & format, Args const & ... args) noexcept
{
    tracing::Logging::Log(tracing::LogCategory::Debug, path, line, functionName, format, args ...);
}

inline void LogInfo(const std::string & path, int line, const std::string & functionName, const std::string & message)
{
    tracing::Logging::Log(tracing::LogCategory::Information, path, line, functionName, message);
}
template <typename ... Args>
void LogInfo(const std::string & path, int line, const std::string & functionName, const std::string & format, Args const & ... args) noexcept
{
    tracing::Logging::Log(tracing::LogCategory::Information, path, line, functionName, format, args ...);
}

inline void LogWarning(const std::string & path, int line, const std::string & functionName, const std::string & message)
{
    tracing::Logging::Log(tracing::LogCategory::Warning, path, line, functionName, message);
}
template <typename ... Args>
void LogWarning(const std::string & path, int line, const std::string & functionName, const std::string & format, Args const & ... args) noexcept
{
    tracing::Logging::Log(tracing::LogCategory::Warning, path, line, functionName, format, args ...);
}

inline void LogError(const std::string & path, int line, const std::string & functionName, const std::string & message)
{
    tracing::Logging::Log(tracing::LogCategory::Error, path, line, functionName, message);
}
template <typename ... Args>
static void LogError(const std::string & path, int line, const std::string & functionName, const std::string & format, Args const & ... args) noexcept
{
    tracing::Logging::Log(tracing::LogCategory::Error, path, line, functionName, format, args ...);
}

inline void LogFatal(const std::string & path, int line, const std::string & functionName, const std::string & message)
{
    tracing::Logging::Log(tracing::LogCategory::Fatal, path, line, functionName, message);
}
template <typename ... Args>
static void LogFatal(const std::string & path, int line, const std::string & functionName, const std::string & format, Args const & ... args) noexcept
{
    tracing::Logging::Log(tracing::LogCategory::Fatal, path, line, functionName, format, args ...);
}
