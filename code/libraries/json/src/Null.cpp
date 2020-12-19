#include "json/Null.h"
#include "Parse.h"
#include "utility/Deserialization.h"
#include "utility/Serialization.h"

namespace json
{

static constexpr size_t IndentSize = 4;

Null::Null()
{
}

bool Null::Deserialize(std::istream & stream)
{
    Token token = GetToken(stream);
    switch (token.type)
    {
        case TokenType::NullToken:
            return true;
        case TokenType::TrueToken:
        case TokenType::FalseToken:
        case TokenType::InvalidToken:
        case TokenType::CurlyBraceOpen:
        case TokenType::CurlyBraceClose:
        case TokenType::SquareBracketOpen:
        case TokenType::SquareBracketClose:
        case TokenType::Comma:
        case TokenType::Colon:
        case TokenType::Number:
        case TokenType::QuotedString:
        default:
            return false;
    }
}

void Null::Serialize(std::ostream & stream, int indentDepth, bool indentInitial) const
{
    if (indentInitial)
    {
        for (int i = 0; i < indentDepth; i++)
        {
            stream << std::string(IndentSize, ' ');
        }
    }
    stream << "null";
}

} // namespace json
