#include "tracing/TraceCategory.h"

#include "utility/EnumSerialization.h"
#include "tracing/TraceEntry.h"

namespace serialization {

template<>
const utility::BidirectionalMap<tracing::TraceCategory, std::string> EnumSerializationMap<tracing::TraceCategory>::ConversionMap = {
    { tracing::TraceCategory::SscfBegin, "SscfBeg"},
    { tracing::TraceCategory::SscfEnd, "SscfEnd"},
    { tracing::TraceCategory::SscfEvent, "SscfEv"},
    { tracing::TraceCategory::SscfLib, "SscfLib"},
    { tracing::TraceCategory::DriverLayer, "Drvr"},
    { tracing::TraceCategory::Physical, "Phys"},
    { tracing::TraceCategory::ApplicationLayer, "Appl"},
    { tracing::TraceCategory::ControlLayer, "Ctrl"},
    { tracing::TraceCategory::PipelineIn, "PlIn"},
    { tracing::TraceCategory::FunctionEnter, "Enter"},
    { tracing::TraceCategory::FunctionLeave, "Leave"},
    { tracing::TraceCategory::CanNmtDbt, "NmtDbt"},
    { tracing::TraceCategory::Information, "Info"},
    { tracing::TraceCategory::Log, "Log"},
    { tracing::TraceCategory::Interrupt, "Interrupt"},
    { tracing::TraceCategory::BistPostInfo, "BistPostInfo"},
    { tracing::TraceCategory::SscfCan, "SscfCan"},
    { tracing::TraceCategory::BitfieldDriver, "BitfieldDriver"},
    { tracing::TraceCategory::StartupShutdown, "StartupShutdown"},
    { tracing::TraceCategory::Cat00001000, "Cat00001000"},
    { tracing::TraceCategory::ResultFlow, "ResultFlow"},
    { tracing::TraceCategory::Cat00000400, "Cat00000400"},
    { tracing::TraceCategory::Cat00000200, "Cat00000200"},
    { tracing::TraceCategory::Cat00000100, "Cat00000100"},
    { tracing::TraceCategory::Cat00000080, "Cat00000080"},
    { tracing::TraceCategory::Cat00000040, "Cat00000040"},
    { tracing::TraceCategory::Cat00000020, "Cat00000020"},
    { tracing::TraceCategory::Cat00000010, "Cat00000010"},
    { tracing::TraceCategory::Cat00000008, "Cat00000008"},
    { tracing::TraceCategory::Data, "Data"},
    { tracing::TraceCategory::Flow, "Flow"},
    { tracing::TraceCategory::Debug, "Debug"},
};

} // namespace serialization

namespace tracing {

std::ostream & operator << (std::ostream & stream, const TraceCategory & value)
{
    return stream << serialization::Serialize(value, "????");
}

std::vector<TraceCategory> EnumTraceCategories()
{
    return serialization::EnumSerializationMap<TraceCategory>::GetKeys();
}

} // namespace tracing
