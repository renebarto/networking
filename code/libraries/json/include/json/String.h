#pragma once

#include "json/Value.h"

namespace json {

class String : public Value
{
public:
    String();
    String(const std::string & value);
    virtual ~String() {};

    virtual bool Deserialize(std::istream & stream) override;
    virtual void Serialize(std::ostream & stream, int indentDepth = 0, bool indentInitial = true) const override;

    void Clear() { m_value.clear(); }
    virtual ValueType Type() const override { return ValueType::String; }
    const std::string & GetValue() const { return m_value; }
    void SetValue(const std::string & value) { m_value = value; }

private:
    std::string m_value;
};

} // namespace json
