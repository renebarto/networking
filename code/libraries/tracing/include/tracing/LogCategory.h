#pragma once

#include <functional>
#include <string>
#include <vector>

namespace tracing {

enum class LogCategory : uint16_t
{
    Trace = 0x01,
    Debug = 0x02,
    Information = 0x04,
    Warning = 0x08,
    Error = 0x10,
    Fatal = 0x20,
};

std::ostream & operator << (std::ostream & stream, const LogCategory & value);

std::vector<LogCategory> EnumLogCategories();

} // namespace tracing
