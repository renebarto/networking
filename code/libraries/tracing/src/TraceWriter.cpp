#include "tracing/TraceWriter.h"

#include <sstream>

#include "osal/ThreadFunctions.h"
#include "utility/Serialization.h"

namespace tracing {

TraceWriter::TraceWriter(LineWriter & writer)
    : m_writer(writer)
{
}

void TraceWriter::Trace(
    osal::EpochTime timestamp,
    TraceCategory category,
    const std::string & fileName, 
    int line, 
    const std::string & functionName, 
    const std::string & msg)
{
    std::ostringstream stream;
    stream << osal::Clock::ToString(timestamp) << "|" << category << "|" << fileName << ":" << line << "|" << functionName << "|" << osal::GetThreadNameSelf() << "|" << msg;
    m_writer.WriteLine(stream.str());
}

} // namespace tracing
