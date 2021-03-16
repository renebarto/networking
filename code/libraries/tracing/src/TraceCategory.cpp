#include "tracing/TraceCategory.h"

#include "utility/EnumSerialization.h"
#include "tracing/TraceEntry.h"

namespace serialization {

template<>
const utility::BidirectionalMap<tracing::TraceCategory, std::string> EnumSerializationMap<tracing::TraceCategory>::ConversionMap = {
    { tracing::TraceCategory::FunctionEnter, "Enter"},
    { tracing::TraceCategory::FunctionLeave, "Leave"},
    { tracing::TraceCategory::Startup, "Start"},
    { tracing::TraceCategory::Shutdown, "Shtdn"},
    { tracing::TraceCategory::Log, "Log  "},
    { tracing::TraceCategory::Message, "Messg"},
    { tracing::TraceCategory::Data, "Data "},
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
