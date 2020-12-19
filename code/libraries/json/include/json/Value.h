#pragma once

#include <iostream>
#include <memory>
#include "utility/EnumSerialization.h"

namespace json {

enum class ValueType
{
    Null,
    Boolean,
    String,
    Number,
    Object,
    Array,
};

inline std::ostream & operator << (std::ostream & stream, ValueType value)
{
    return stream << serialization::Serialize(value);
}

class Value
{
public:
    Value() {}
    virtual ~Value() {};

    virtual bool Deserialize(std::istream & stream) = 0;
    virtual void Serialize(std::ostream & stream, int indentDepth = 0, bool indentInitial = true) const = 0;

    virtual ValueType Type() const = 0;

private:
};
using ValuePtr = std::shared_ptr<Value>;

ValuePtr Parse(std::istream & stream);

inline void PrintTo(std::ostream & stream, const Value & value)
{
    value.Serialize(stream, 0);
}

} // namespace json
