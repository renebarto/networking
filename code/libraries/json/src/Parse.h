#include "json/Value.h"
#include "utility/Deserialization.h"
#include "utility/Serialization.h"

namespace json
{

enum class TokenType
{
    InvalidToken,
    NullToken,
    TrueToken,
    FalseToken,
    CurlyBraceOpen,
    CurlyBraceClose,
    SquareBracketOpen,
    SquareBracketClose,
    Comma,
    Colon,
    Number,
    QuotedString,
};

struct Token
{
    TokenType type;
    std::string value;
};

bool GetTerm(std::istream & stream, std::string & term);
Token GetToken(std::istream & stream);
ValuePtr Parse(std::istream & stream);

} // namespace json
