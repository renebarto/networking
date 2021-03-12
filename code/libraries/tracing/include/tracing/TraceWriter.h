#pragma once

#include "osal/Clock.h"
#include "tracing/LineWriter.h"
#include "tracing/TraceCategory.h"

namespace tracing {

class TraceWriter
{
private:
    LineWriter & m_writer;

public:
    TraceWriter(LineWriter & writer);

    void Trace(
        osal::EpochTime timestamp,
        TraceCategory category,
        const std::string & fileName, 
        int line, 
        const std::string & functionName, 
        const std::string & msg);
};

} // namespace tracing