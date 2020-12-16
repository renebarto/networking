#pragma once

#include <functional>
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
    static void Trace(TraceCategory category, const std::string & fileName, int line, const std::string & functionName, const std::string & message);
    template <typename ... Args>
    static void Trace(TraceCategory category, const std::string & fileName, int line, const std::string & functionName, const std::string & format, Args const & ... args) noexcept
    {
        if (IsTraceCategoryEnabled(category))
        {
            std::string str;
            utility::Format(str, format, args ...);
            Trace(category, fileName, line, functionName, str);
        }
    }

private:
    static TraceFunction m_traceFunc;
    static IsTraceCategoryEnabledFunction m_isTraceCategoryEnabledFunc;

    std::string m_fileName;
    std::string m_functionName;
};

std::ostream & operator << (std::ostream & stream, const TraceCategory & value);

class Tracer {
public:
    static void Trace(const std::string & fileName, int line, const std::string & functionName, const utility::Error & error);
    static void Fatal(const std::string & fileName, int line, const std::string & functionName, const utility::Error & error);
    static void Throw(const std::string & fileName, int line, const std::string & functionName, const utility::Error & error);
    static void Throw(const std::string & fileName, int line, const std::string & functionName, const utility::GenericError & error);
};

} // namespace tracing

inline void TraceDebug(const std::string & fileName, int line, const std::string & functionName, const std::string & message)
{
    tracing::Tracing::Trace(tracing::TraceCategory::Debug, fileName, line, functionName, message);
}
template <typename ... Args>
void TraceDebug(const std::string & fileName, int line, const std::string & functionName, const std::string & format, Args const & ... args) noexcept
{
    tracing::Tracing::Trace(tracing::TraceCategory::Debug, fileName, line, functionName, format, args ...);
}

inline void TraceInfo(const std::string & fileName, int line, const std::string & functionName, const std::string & message)
{
    tracing::Tracing::Trace(tracing::TraceCategory::Information, fileName, line, functionName, message);
}
template <typename ... Args>
void TraceInfo(const std::string & fileName, int line, const std::string & functionName, const std::string & format, Args const & ... args) noexcept
{
    tracing::Tracing::Trace(tracing::TraceCategory::Information, fileName, line, functionName, format, args ...);
}

inline void TraceError(const std::string & fileName, int line, const std::string & functionName, const std::string & message)
{
    tracing::Tracing::Trace(tracing::TraceCategory::Error, fileName, line, functionName, message);
}
template <typename ... Args>
static void TraceError(const std::string & fileName, int line, const std::string & functionName, const std::string & format, Args const & ... args) noexcept
{
    tracing::Tracing::Trace(tracing::TraceCategory::Error, fileName, line, functionName, format, args ...);
}

