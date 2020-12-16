#include "json/Number.h"
#include "Parse.h"
#include "utility/Deserialization.h"
#include "utility/Serialization.h"

namespace json
{

static constexpr size_t IndentSize = 4;
static constexpr int NumDigitsFloat = 8;

Number::Number()
    : m_value()
{
}

Number::Number(const std::string & value)
    : m_value(value)
{
}

Number::Number(int8_t value)
    : m_value(utility::Serialize(value))
{
}

Number::Number(uint8_t value)
    : m_value(utility::Serialize(value))
{
}

Number::Number(int16_t value)
    : m_value(utility::Serialize(value))
{
}

Number::Number(uint16_t value)
    : m_value(utility::Serialize(value))
{
}

Number::Number(int32_t value)
    : m_value(utility::Serialize(value))
{
}

Number::Number(uint32_t value)
    : m_value(utility::Serialize(value))
{
}

Number::Number(int64_t value)
    : m_value(utility::Serialize(value))
{
}

Number::Number(uint64_t value)
    : m_value(utility::Serialize(value))
{
}

Number::Number(float value)
    : m_value(utility::Serialize(value, 0, NumDigitsFloat, utility::FloatingPointRepresentation::Mixed))
{
}

Number::Number(double value)
    : m_value(utility::Serialize(value))
{
}

bool Number::GetValue(int8_t & value) const
{
    value = {};
    return utility::Deserialize(m_value, value);
}

bool Number::GetValue(uint8_t & value) const
{
    value = {};
    return utility::Deserialize(m_value, value);
}

bool Number::GetValue(int16_t & value) const
{
    value = {};
    return utility::Deserialize(m_value, value);
}

bool Number::GetValue(uint16_t & value) const
{
    value = {};
    return utility::Deserialize(m_value, value);
}

bool Number::GetValue(int32_t & value) const
{
    value = {};
    return utility::Deserialize(m_value, value);
}

bool Number::GetValue(uint32_t & value) const
{
    value = {};
    return utility::Deserialize(m_value, value);
}

bool Number::GetValue(int64_t & value) const
{
    value = {};
    return utility::Deserialize(m_value, value);
}

bool Number::GetValue(uint64_t & value) const
{
    value = {};
    return utility::Deserialize(m_value, value);
}

bool Number::GetValue(float & value) const
{
    value = {};
    return utility::Deserialize(m_value, value);
}

bool Number::GetValue(double & value) const
{
    value = {};
    return utility::Deserialize(m_value, value);
}

void Number::SetValue(int8_t value)
{
    m_value = utility::Serialize(value);
}

void Number::SetValue(uint8_t value)
{
    m_value = utility::Serialize(value);
}

void Number::SetValue(int16_t value)
{
    m_value = utility::Serialize(value);
}

void Number::SetValue(uint16_t value)
{
    m_value = utility::Serialize(value);
}

void Number::SetValue(int32_t value)
{
    m_value = utility::Serialize(value);
}

void Number::SetValue(uint32_t value)
{
    m_value = utility::Serialize(value);
}

void Number::SetValue(int64_t value)
{
    m_value = utility::Serialize(value);
}

void Number::SetValue(uint64_t value)
{
    m_value = utility::Serialize(value);
}

void Number::SetValue(float value)
{
    m_value = utility::Serialize(value, 0, NumDigitsFloat, utility::FloatingPointRepresentation::Mixed);
}

void Number::SetValue(double value)
{
    m_value = utility::Serialize(value);
}

bool Number::Deserialize(std::istream & stream)
{
    Token token = GetToken(stream);
    switch (token.type)
    {
        case TokenType::Number:
            m_value = token.value;
            return true;
        default:
            m_value = {};
            return false;
    }
}

void Number::Serialize(std::ostream & stream, int indentDepth, bool indentInitial) const
{
    if (indentInitial)
    {
        for (int i = 0; i < indentDepth; i++)
        {
            stream << std::string(IndentSize, ' ');
        }
    }
    stream << m_value;
}

} // namespace json
