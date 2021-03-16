#pragma once

#include <functional>
#include <mutex>
#include <string>
#include "osal/Clock.h"
#include "utility/Format.h"
#include "tracing/CategorySet.h"
#include "tracing/TraceCategory.h"

#define TRACE_LOCATION __FILE__, __LINE__, __func__

namespace utility {

class Error;
class GenericError;

}

namespace tracing {

using TraceFunction = std::function<
    void (osal::EpochTime timestamp,
          TraceCategory category,
          const std::string & fileName, 
          int line, 
          const std::string & functionName, 
          const std::string & msg)>;

using IsTraceCategoryEnabledFunction = std::function<bool(TraceCategory category)>;

void SetDefaultTraceFilter(const CategorySet<TraceCategory> & defaultFilter);
CategorySet<TraceCategory> GetDefaultTraceFilter();

class Tracing
{
private:
    friend void SetDefaultTraceFilter(const CategorySet<TraceCategory> & defaultFilter);
    friend CategorySet<TraceCategory> GetDefaultTraceFilter();

    static TraceFunction m_traceFunc;
    static IsTraceCategoryEnabledFunction m_isTraceCategoryEnabledFunc;
    static CategorySet<TraceCategory> m_defaultTraceFilter;

    typedef std::recursive_mutex Mutex;
    typedef std::lock_guard<Mutex> Lock;
    static Mutex m_traceMutex;

    std::string m_fileName;
    std::string m_functionName;

public:
    Tracing() = default;
    virtual ~Tracing() noexcept;
    
    static void SetTracingFunction(TraceFunction traceFunc);
    static void SetTracingEnabledFunction(IsTraceCategoryEnabledFunction enabledFunc);
    
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
};

} // namespace tracing

inline void TraceFuncEnter(const std::string & path, int line, const std::string & functionName, const std::string & message)
{
    tracing::Tracing::Trace(tracing::TraceCategory::FunctionEnter, path, line, functionName, message);
}
template <typename ... Args>
void TraceFuncEnter(const std::string & path, int line, const std::string & functionName, const std::string & format, Args const & ... args) noexcept
{
    tracing::Tracing::Trace(tracing::TraceCategory::FunctionEnter, path, line, functionName, format, args ...);
}

inline void TraceFuncLeave(const std::string & path, int line, const std::string & functionName, const std::string & message)
{
    tracing::Tracing::Trace(tracing::TraceCategory::FunctionLeave, path, line, functionName, message);
}
template <typename ... Args>
void TraceFuncLeave(const std::string & path, int line, const std::string & functionName, const std::string & format, Args const & ... args) noexcept
{
    tracing::Tracing::Trace(tracing::TraceCategory::FunctionLeave, path, line, functionName, format, args ...);
}

inline void TraceStartup(const std::string & path, int line, const std::string & functionName, const std::string & message)
{
    tracing::Tracing::Trace(tracing::TraceCategory::Startup, path, line, functionName, message);
}

inline void TraceShutdown(const std::string & path, int line, const std::string & functionName, const std::string & message)
{
    tracing::Tracing::Trace(tracing::TraceCategory::Shutdown, path, line, functionName, message);
}

inline void TraceMessage(const std::string & path, int line, const std::string & functionName, const std::string & message)
{
    tracing::Tracing::Trace(tracing::TraceCategory::Message, path, line, functionName, message);
}

template <typename ... Args>
void TraceMessage(const std::string & path, int line, const std::string & functionName, const std::string & format, Args const & ... args) noexcept
{
    tracing::Tracing::Trace(tracing::TraceCategory::Message, path, line, functionName, format, args ...);
}
inline void TraceData(const std::string & path, int line, const std::string & functionName, const std::string & message)
{
    tracing::Tracing::Trace(tracing::TraceCategory::Data, path, line, functionName, message);
}

template <typename ... Args>
void TraceData(const std::string & path, int line, const std::string & functionName, const std::string & format, Args const & ... args) noexcept
{
    tracing::Tracing::Trace(tracing::TraceCategory::Data, path, line, functionName, format, args ...);
}

inline void TraceDebug(const std::string & path, int line, const std::string & functionName, const std::string & message)
{
    tracing::Tracing::Trace(tracing::TraceCategory::Debug, path, line, functionName, message);
}
template <typename ... Args>
void TraceDebug(const std::string & path, int line, const std::string & functionName, const std::string & format, Args const & ... args) noexcept
{
    tracing::Tracing::Trace(tracing::TraceCategory::Debug, path, line, functionName, format, args ...);
}

