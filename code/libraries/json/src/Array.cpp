#include "json/Array.h"
#include "Parse.h"
#include "json/Boolean.h"
#include "json/Null.h"
#include "json/Number.h"
#include "json/String.h"

namespace json
{

static constexpr size_t IndentSize = 4;

Array::Array()
    : m_elements()
{

}

Array::Array(const std::initializer_list<ValuePtr> & elements)
: m_elements()
{
    for (auto & element : elements)
    {
        AddValue(element);
    }
}

void Array::AddValue(ValuePtr value)
{
    m_elements.push_back(value);
}

bool Array::Deserialize(std::istream & stream)
{
    std::string key;
    Token token = GetToken(stream);
    if (token.type != TokenType::SquareBracketOpen)
        return false;

    bool success = true;
    bool empty = true;
    while (success)
    {
        ValuePtr value;
        value = Parse(stream);
        if (value != nullptr)
        {
            AddValue(value);
            empty = false;
        }
        else
            return empty;
        token = GetToken(stream);
        if (token.type == TokenType::SquareBracketClose)
            return true;
        if (token.type != TokenType::Comma)
        {
            while (token.type != TokenType::InvalidToken)
            {
                token = GetToken(stream);
                if (token.type == TokenType::SquareBracketClose)
                    break;
            }
            return false;
        }
    }

    token = GetToken(stream);
    return (token.type == TokenType::CurlyBraceClose);
}

void Array::Serialize(std::ostream & stream, int indentDepth, bool indentInitial) const
{
    if (indentInitial)
    {
        for (int i = 0; i < indentDepth; i++)
        {
            stream << std::string(IndentSize, ' ');
        }
    }
    stream << '[' << std::endl;
    ConstIterator iterator = GetIterator();
    while (!iterator.AtEnd())
    {
        (*iterator)->Serialize(stream, indentDepth + 1);
        iterator++;
        if (!iterator.AtEnd())
            stream << ',';
        stream << std::endl;
    }
    for (int i = 0; i < indentDepth; i++)
    {
        stream << std::string(IndentSize, ' ');
    }
    stream << ']';
}

} // namespace json
