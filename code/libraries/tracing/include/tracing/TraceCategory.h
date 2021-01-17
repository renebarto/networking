#pragma once

#include <functional>
#include <string>
#include <vector>

namespace tracing {

enum class TraceCategory : uint16_t
{
    FunctionEnter = 0x01,
    FunctionLeave = 0x02,
    Startup = 0x04,
    Shutdown = 0x08,
    Log = 0x10,
    Message = 0x20,
    Data = 0x40,
    None = 0x00,
    All = 0x7F,
};

std::ostream & operator << (std::ostream & stream, const TraceCategory & value);

std::vector<TraceCategory> EnumTraceCategories();

} // namespace tracing
