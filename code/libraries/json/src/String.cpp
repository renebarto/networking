#include "json/String.h"
#include "Parse.h"
#include "utility/Deserialization.h"
#include "utility/Serialization.h"

namespace json
{

static constexpr size_t IndentSize = 4;

String::String()
    : m_value()
{
}

String::String(const std::string & value)
    : m_value(value)
{
}

bool String::Deserialize(std::istream & stream)
{
    Token token = GetToken(stream);
    switch (token.type)
    {
        case TokenType::QuotedString:
            m_value = token.value;
            return true;
        default:
            m_value = {};
            return false;
    }
}

void String::Serialize(std::ostream & stream, int indentDepth, bool indentInitial) const
{
    if (indentInitial)
    {
        for (int i = 0; i < indentDepth; i++)
        {
            stream << std::string(IndentSize, ' ');
        }
    }
    stream << '"' << m_value << '"';
}

} // namespace json
