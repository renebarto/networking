#include "tracing/ConsoleLineWriter.h"

#include "osal/Console.h"
#include "tracing/TraceEntry.h"

static osal::Console s_console;

namespace tracing {

void ConsoleLineWriter::WriteLine(const std::string & line)
{
    s_console << line << std::endl;
}

} // namespace tracing
