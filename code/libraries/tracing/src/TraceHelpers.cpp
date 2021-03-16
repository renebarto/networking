#include "tracing/TraceHelpers.h"

#include <algorithm>
#include "tracing/TraceEntry.h"

std::string ExtractFileName(const std::string & path)
{
    std::string convertedPath = path;
    std::replace(convertedPath.begin(), convertedPath.end(), '\\', '/');
    return convertedPath.substr(convertedPath.rfind("/") + 1u);
}

std::string RelativePath(const std::string & path)
{
    std::string convertedPath = path;
    std::replace(convertedPath.begin(), convertedPath.end(), '\\', '/');
    std::string rootPath = ROOT_PATH;
    std::replace(rootPath.begin(), rootPath.end(), '\\', '/');
    if (convertedPath.find(rootPath) == 0)
    {
        convertedPath = convertedPath.substr(std::string(ROOT_PATH).length());
    }
    if ((convertedPath.length() >= 1) && (convertedPath[0] == '/'))
        convertedPath = convertedPath.substr(1);
    return convertedPath;
}
