#include "tracing/LogWriter.h"

#include <sstream>

#include "osal/ThreadFunctions.h"
#include "utility/Serialization.h"

namespace tracing {

LogWriter::LogWriter(LineWriter & writer)
    : m_writer(writer)
{
}

void LogWriter::Log(
    osal::EpochTime timestamp,
    LogCategory category,
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
