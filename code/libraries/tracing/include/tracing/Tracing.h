#pragma once

#include <functional>
#include <string>
#include "utility/Format.h"

namespace tracing {

enum class TraceCategory {
    FunctionBegin,
    FunctionEnd,
    Information,
    Log,
    Startup,
    Shutdown,
    Debug,
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

} // namespace tracing
