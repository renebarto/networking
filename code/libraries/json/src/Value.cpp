#include "json/Value.h"

#include "utility/EnumSerialization.h"

namespace utility {

template<>
const BidirectionalMap<json::ValueType, std::string> EnumSerializationMap<json::ValueType>::ConversionMap = 
{
    {json::ValueType::Null, "Null"},
    {json::ValueType::Boolean, "Boolean"},
    {json::ValueType::String, "String"},
    {json::ValueType::Number, "Number"},
    {json::ValueType::Object, "Object"},
    {json::ValueType::Array, "Array"},
};

} // namespace utility
