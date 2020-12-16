#pragma once

#include "json/Value.h"

namespace json {

class Number : public Value
{
public:
    Number();
    Number(const std::string & value);
    Number(int8_t value);
    Number(uint8_t value);
    Number(int16_t value);
    Number(uint16_t value);
    Number(int32_t value);
    Number(uint32_t value);
    Number(int64_t value);
    Number(uint64_t value);
    Number(float value);
    Number(double value);
    virtual ~Number() {};

    virtual bool Deserialize(std::istream & stream) override;
    virtual void Serialize(std::ostream & stream, int indentDepth = 0, bool indentInitial = true) const override;

    void Clear() { m_value.clear(); }
    virtual ValueType Type() const override { return ValueType::Number; }
    const std::string & GetValue() const { return m_value; }
    bool GetValue(int8_t & value) const;
    bool GetValue(uint8_t & value) const;
    bool GetValue(int16_t & value) const;
    bool GetValue(uint16_t & value) const;
    bool GetValue(int32_t & value) const;
    bool GetValue(uint32_t & value) const;
    bool GetValue(int64_t & value) const;
    bool GetValue(uint64_t & value) const;
    bool GetValue(float & value) const;
    bool GetValue(double & value) const;
    void SetValue(const std::string & value) { m_value = value; }
    void SetValue(int8_t value);
    void SetValue(uint8_t value);
    void SetValue(int16_t value);
    void SetValue(uint16_t value);
    void SetValue(int32_t value);
    void SetValue(uint32_t value);
    void SetValue(int64_t value);
    void SetValue(uint64_t value);
    void SetValue(float value);
    void SetValue(double value);

private:
    std::string m_value;
};

} // namespace json
