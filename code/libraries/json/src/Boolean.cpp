#include "json/Boolean.h"
#include "Parse.h"
#include "utility/Deserialization.h"
#include "utility/Serialization.h"

namespace json
{

static constexpr size_t IndentSize = 4;

Boolean::Boolean()
    : m_value()
{
}

Boolean::Boolean(bool value)
    : m_value(value)
{
}

bool Boolean::Deserialize(std::istream & stream)
{
    Token token = GetToken(stream);
    switch (token.type)
    {
        case TokenType::FalseToken:
            m_value = false;
            return true;
        case TokenType::TrueToken:
            m_value = true;
            return true;
        default:
            m_value = {};
            return false;
    }
}

void Boolean::Serialize(std::ostream & stream, int indentDepth, bool indentInitial) const
{
    if (indentInitial)
    {
        for (int i = 0; i < indentDepth; i++)
        {
            stream << std::string(IndentSize, ' ');
        }
    }
    stream << (m_value ? "true" : "false");
}

} // namespace json
