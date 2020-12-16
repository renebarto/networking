#pragma once

#include "json/Value.h"

namespace json {

class Boolean : public Value
{
public:
    Boolean();
    Boolean(bool value);
    virtual ~Boolean() {};

    virtual bool Deserialize(std::istream & stream) override;
    virtual void Serialize(std::ostream & stream, int indentDepth = 0, bool indentInitial = true) const override;

    virtual ValueType Type() const override { return ValueType::Boolean; }
    bool GetValue() const { return m_value; }
    void SetValue(bool value) { m_value = value; }

private:
    bool m_value;
};

} // namespace json
