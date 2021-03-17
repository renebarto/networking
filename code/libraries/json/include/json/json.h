#pragma once

#include "json/Value.h"
#include "json/Array.h"
#include "json/Object.h"
#include "json/String.h"
#include "json/Number.h"
#include "json/Null.h"
#include "json/Boolean.h"
#include "json/Create.h"
#include "json/Serialization.h"

namespace json {

inline bool IsArray(ValuePtr value)
{
    return value->Type() == ValueType::Array;
}

inline bool IsObject(ValuePtr value)
{
    return value->Type() == ValueType::Object;
}

inline bool IsBoolean(ValuePtr value)
{
    return value->Type() == ValueType::Boolean;
}

inline bool IsNumber(ValuePtr value)
{
    return value->Type() == ValueType::Number;
}

inline bool IsString(ValuePtr value)
{
    return value->Type() == ValueType::String;
}

inline bool IsNull(ValuePtr value)
{
    return value->Type() == ValueType::Null;
}

inline std::shared_ptr<Array> ToArray(ValuePtr value)
{
    return std::dynamic_pointer_cast<Array>(value);
}

inline std::shared_ptr<Object> ToObject(ValuePtr value)
{
    return std::dynamic_pointer_cast<Object>(value);
}

inline std::shared_ptr<Boolean> ToBoolean(ValuePtr value)
{
    return std::dynamic_pointer_cast<Boolean>(value);
}

inline std::shared_ptr<Number> ToNumber(ValuePtr value)
{
    return std::dynamic_pointer_cast<Number>(value);
}

inline std::shared_ptr<String> ToString(ValuePtr value)
{
    return std::dynamic_pointer_cast<String>(value);
}

inline std::shared_ptr<Null> ToNull(ValuePtr value)
{
    return std::dynamic_pointer_cast<Null>(value);
}

} // namespace json