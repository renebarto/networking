#include "tracing/TraceHelpers.h"

#include <algorithm>

std::string ExtractFileName(const std::string & path)
{
    std::string convertedPath = path;
    std::replace(convertedPath.begin(), convertedPath.end(), '\\', '/');
    return convertedPath.substr(convertedPath.rfind("/") + 1u);
}
