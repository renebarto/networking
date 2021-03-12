#pragma once

#include <string>

namespace tracing {

class LineWriter {
public:
    virtual ~LineWriter() = default;

    virtual void WriteLine(const std::string & line) = 0;
};

} // namespace tracing 
