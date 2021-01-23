#pragma once

#include <functional>
#include <string>
#include <vector>

namespace tracing {

enum class LogCategory : uint16_t
{
    Information = 0x01,
    Warning = 0x02,
    Error = 0x04,
    Fatal = 0x08,
    None = 0x00,
    All = 0x0F,
};

std::ostream & operator << (std::ostream & stream, const LogCategory & value);

std::vector<LogCategory> EnumLogCategories();

} // namespace tracing
