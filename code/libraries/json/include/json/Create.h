#pragma once

#include <iostream>
#include <memory>
#include "json/Value.h"
#include "json/Boolean.h"
#include "json/Null.h"
#include "json/Number.h"
#include "json/Object.h"
#include "json/String.h"

namespace json {

template <class T>
inline ValuePtr CreateValue(const T & value)
{
    return nullptr;
}
template <>
inline ValuePtr CreateValue<std::string>(const std::string & value)
{
    return std::make_shared<String>(value);
}
inline ValuePtr CreateValue(const char * value)
{
    return std::make_shared<String>(value);
}
template <>
inline ValuePtr CreateValue<bool>(const bool & value)
{
    return std::make_shared<Boolean>(value);
}
template <>
inline ValuePtr CreateValue<nullptr_t>(const nullptr_t & /*value*/)
{
    return std::make_shared<Null>();
}
template <>
inline ValuePtr CreateValue(const int8_t & value)
{
    return std::make_shared<Number>(value);
}
template <>
inline ValuePtr CreateValue(const uint8_t & value)
{
    return std::make_shared<Number>(value);
}
template <>
inline ValuePtr CreateValue(const int16_t & value)
{
    return std::make_shared<Number>(value);
}
template <>
inline ValuePtr CreateValue(const uint16_t & value)
{
    return std::make_shared<Number>(value);
}
template <>
inline ValuePtr CreateValue(const int32_t & value)
{
    return std::make_shared<Number>(value);
}
template <>
inline ValuePtr CreateValue(const uint32_t & value)
{
    return std::make_shared<Number>(value);
}
template <>
inline ValuePtr CreateValue(const int64_t & value)
{
    return std::make_shared<Number>(value);
}
template <>
inline ValuePtr CreateValue(const uint64_t & value)
{
    return std::make_shared<Number>(value);
}
template <>
inline ValuePtr CreateValue(const float & value)
{
    return std::make_shared<Number>(value);
}
template <>
inline ValuePtr CreateValue(const double & value)
{
    return std::make_shared<Number>(value);
}

} // namespace json
