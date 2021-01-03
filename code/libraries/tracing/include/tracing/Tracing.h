#pragma once

#include <functional>
#include <mutex>
#include <string>
#include "utility/Format.h"

namespace utility {

class Error;
class GenericError;

}

namespace tracing {

enum class TraceCategory {
    FunctionBegin,
    FunctionEnd,
    Information,
    Log,
    Startup,
    Shutdown,
    Debug,
    Error,
};

using TraceFunction = std::function<
    void (TraceCategory category,
          const std::string & fileName, 
          int line, 
          const std::string & functionName, 
          const std::string & msg)>;

using IsTraceCategoryEnabledFunction = std::function<bool(TraceCategory category)>;

class Tracing
{
public:
    Tracing() = default;
    virtual ~Tracing() noexcept;
    
    static void SetTracingFunctions(TraceFunction traceFunc, IsTraceCategoryEnabledFunction enabledFunc);
    
    static bool IsTraceCategoryEnabled(TraceCategory category);
    static void Trace(TraceCategory category, const std::string & path, int line, const std::string & functionName, const std::string & message);
    template <typename ... Args>
    static void Trace(TraceCategory category, const std::string & path, int line, const std::string & functionName, const std::string & format, Args const & ... args) noexcept
    {
        if (IsTraceCategoryEnabled(category))
        {
            std::string str;
            utility::Format(str, format, args ...);
            Trace(category, path, line, functionName, str);
        }
    }

private:
    static TraceFunction m_traceFunc;
    static IsTraceCategoryEnabledFunction m_isTraceCategoryEnabledFunc;
    typedef std::recursive_mutex Mutex;
    typedef std::lock_guard<Mutex> Lock;
    static Mutex m_traceMutex;

    std::string m_fileName;
    std::string m_functionName;
};

std::ostream & operator << (std::ostream & stream, const TraceCategory & value);

class Tracer {
public:
    static void Trace(const std::string & path, int line, const std::string & functionName, const utility::Error & error);
    static void Trace(const std::string & path, int line, const std::string & functionName, const utility::GenericError & error);
    static void Fatal(const std::string & path, int line, const std::string & functionName, const utility::Error & error);
    static void Fatal(const std::string & path, int line, const std::string & functionName, const utility::GenericError & error);
    static void Throw(const std::string & path, int line, const std::string & functionName, const utility::Error & error);
    static void Throw(const std::string & path, int line, const std::string & functionName, const utility::GenericError & error);
};

} // namespace tracing

inline void TraceDebug(const std::string & path, int line, const std::string & functionName, const std::string & message)
{
    tracing::Tracing::Trace(tracing::TraceCategory::Debug, path, line, functionName, message);
}
template <typename ... Args>
void TraceDebug(const std::string & path, int line, const std::string & functionName, const std::string & format, Args const & ... args) noexcept
{
    tracing::Tracing::Trace(tracing::TraceCategory::Debug, path, line, functionName, format, args ...);
}

inline void TraceInfo(const std::string & path, int line, const std::string & functionName, const std::string & message)
{
    tracing::Tracing::Trace(tracing::TraceCategory::Information, path, line, functionName, message);
}
template <typename ... Args>
void TraceInfo(const std::string & path, int line, const std::string & functionName, const std::string & format, Args const & ... args) noexcept
{
    tracing::Tracing::Trace(tracing::TraceCategory::Information, path, line, functionName, format, args ...);
}

inline void TraceError(const std::string & path, int line, const std::string & functionName, const std::string & message)
{
    tracing::Tracing::Trace(tracing::TraceCategory::Error, path, line, functionName, message);
}
template <typename ... Args>
static void TraceError(const std::string & path, int line, const std::string & functionName, const std::string & format, Args const & ... args) noexcept
{
    tracing::Tracing::Trace(tracing::TraceCategory::Error, path, line, functionName, format, args ...);
}

