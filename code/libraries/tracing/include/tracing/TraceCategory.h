#pragma once

#include <functional>
#include <string>
#include <vector>

namespace tracing {

enum class TraceCategory : uint32_t
{
    SscfBegin            = 0x80000000u,
    SscfEnd              = 0x40000000u,
    SscfEvent            = 0x20000000u,
    SscfLib              = 0x10000000u,
    DriverLayer          = 0x08000000u,
    Physical             = 0x04000000u,
    ApplicationLayer     = 0x02000000u,
    ControlLayer         = 0x01000000u,
    PipelineIn           = 0x00800000u,
    FunctionEnter        = 0x00400000u,
    FunctionLeave        = 0x00200000u,
    CanNmtDbt            = 0x00100000u,
    Information          = 0x00080000u,
    Log                  = 0x00040000u,   // Used by logging ONLY!
    Interrupt            = 0x00020000u,
    BistPostInfo         = 0x00010000u,
    SscfCan              = 0x00008000u,
    BitfieldDriver       = 0x00004000u,
    StartupShutdown      = 0x00002000u,
    Cat00001000          = 0x00001000u,
    ResultFlow           = 0x00000800u,
    Cat00000400          = 0x00000400u,
    Cat00000200          = 0x00000200u,
    Cat00000100          = 0x00000100u,
    Cat00000080          = 0x00000080u,
    Cat00000040          = 0x00000040u,
    Cat00000020          = 0x00000020u,
    Cat00000010          = 0x00000010u,
    Cat00000008          = 0x00000008u,
    Data                 = 0x00000004u,
    Flow                 = 0x00000002u,   // use this for undefined tracing
    Debug                = 0x00000001u,   // for debug; not used in archived code
    None                 = 0x00000000u,
    All                  = 0xFFFFFFFFu,
};

std::ostream & operator << (std::ostream & stream, const TraceCategory & value);

std::vector<TraceCategory> EnumTraceCategories();

} // namespace tracing
