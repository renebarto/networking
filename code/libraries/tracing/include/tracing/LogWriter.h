#pragma once

#include "osal/Clock.h"
#include "tracing/LineWriter.h"
#include "tracing/TraceCategory.h"

namespace tracing {

class LogWriter
{
private:
    LineWriter & m_writer;

public:
    LogWriter(LineWriter & writer);

    void Log(
        osal::EpochTime timestamp,
        LogCategory category,
        const std::string & fileName, 
        int line, 
        const std::string & functionName, 
        const std::string & msg);
};

} // namespace tracing