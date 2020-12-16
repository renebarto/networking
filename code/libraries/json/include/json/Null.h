#pragma once

#include "json/Value.h"

namespace json {

class Null : public Value
{
public:
    Null();
    virtual ~Null() {};

    virtual bool Deserialize(std::istream & stream) override;
    virtual void Serialize(std::ostream & stream, int indentDepth = 0, bool indentInitial = true) const override;

    virtual ValueType Type() const override { return ValueType::Null; }

private:
};

} // namepsace json
