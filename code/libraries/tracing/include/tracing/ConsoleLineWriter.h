#pragma once

#include "tracing/LineWriter.h"

namespace tracing {

class ConsoleLineWriter
    : public LineWriter
{
public:
    void WriteLine(const std::string & line) override;
};

} // namespace tracing 
