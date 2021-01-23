#include "tracing/LogCategory.h"

#include "utility/EnumSerialization.h"

namespace serialization {

template<>
const utility::BidirectionalMap<tracing::LogCategory, std::string> EnumSerializationMap<tracing::LogCategory>::ConversionMap = {
    { tracing::LogCategory::Information, "Info "},
    { tracing::LogCategory::Warning, "Warn "},
    { tracing::LogCategory::Error, "Error"},
    { tracing::LogCategory::Fatal, "Fatal"},
};

} // namespace serialization

namespace tracing {

std::ostream & operator << (std::ostream & stream, const LogCategory & value)
{
    return stream << serialization::Serialize(value, "????");
}

std::vector<LogCategory> EnumLogCategories()
{
    return serialization::EnumSerializationMap<LogCategory>::GetKeys();
}

} // namespace tracing
