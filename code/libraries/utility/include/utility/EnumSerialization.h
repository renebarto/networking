#pragma once

#include <string>
#include <type_traits>
#include "utility/BidirectionalMap.h"

namespace serialization {

template<typename EnumType>
struct EnumSerializationMap
{
public:
    EnumSerializationMap() {}
    EnumSerializationMap(const EnumSerializationMap &) = delete;
    EnumSerializationMap & operator = (const EnumSerializationMap &) = delete;

    static std::string Serialize(EnumType value, const std::string & defaultValue)
    {
        return ConversionMap.Translate(value, defaultValue);
    }
    static std::string Serialize(EnumType value)
    {
        return ConversionMap[value];
    }
    static bool Deserialize(const std::string & text, EnumType & value)
    {
        try
        {
            value = ConversionMap[text];
        }
        catch (std::runtime_error &)
        {
            value = {};
            return false;
        }
        return true;
    }
    static std::vector<EnumType> GetKeys() { return ConversionMap.GetKeys(); }
    
private:
    static const utility::BidirectionalMap<EnumType, std::string> ConversionMap;
};

template<typename EnumType>
typename std::enable_if<std::is_enum<EnumType>::value, std::string>::type
Serialize(const EnumType & value)
{
    return EnumSerializationMap<EnumType>::Serialize(value);
}

template<typename EnumType>
typename std::enable_if<std::is_enum<EnumType>::value, std::string>::type
Serialize(const EnumType & value, const std::string & defaultValue)
{
    return EnumSerializationMap<EnumType>::Serialize(value, defaultValue);
}

template<typename EnumType>
typename std::enable_if<std::is_enum<EnumType>::value, bool>::type
Deserialize(const std::string & text, EnumType & value)
{
    return EnumSerializationMap<EnumType>::Deserialize(text, value);
}

} // namespace serialization

