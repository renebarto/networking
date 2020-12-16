#include "Parse.h"

#include "utility/StringFunctions.h"
#include "json/Null.h"
#include "json/Boolean.h"
#include "json/String.h"
#include "json/Number.h"
#include "json/Object.h"
#include "json/Array.h"

namespace json
{

static bool IsValidCharacter(char ch)
{
    return std::isalnum(ch) || (ch == '-') || (ch == '+') || (ch == '.');
}

bool GetTerm(std::istream & stream, std::string & term)
{
    term = {};
    char ch;
    if (!stream.get(ch))
        return false;
    while (std::isspace(ch))
    {
        if (!stream.get(ch))
            return false;
    }

    switch (ch)
    {
        case '{':
        case '}':
        case '[':
        case ']':
        case ',':
        case ':':
            term = ch;
            return true;
        case '"':
        {
            std::string result;
            result += ch;
            if (!stream.get(ch))
                return false;
            while (ch != '"')
            {
                if (ch == '\\')
                {
                    if (!stream.get(ch))
                        return false;
                    switch(ch)
                    {
                        case '\"':
                            result += '\"';
                            break;
                        case '\\':
                            result += '\\';
                            break;
                        case '/':
                            result += '/';
                            break;
                        case 'b':
                            result += '\b';
                            break;
                        case 'f':
                            result += '\f';
                            break;
                        case 'n':
                            result += '\n';
                            break;
                        case 'r':
                            result += '\r';
                            break;
                        case 't':
                            result += '\t';
                            break;
                        case 'u':
                        {
                            std::string hexDigits;
                            for (int i = 0; i < 4; ++i)
                            {
                                if (!stream.get(ch))
                                    return false;
                                hexDigits += ch;
                            }
                            uint16_t hexCode;
                            utility::Deserialize(hexDigits, hexCode, 16);
                            if ((hexCode & 0xFF80) == 0)
                            {
                                result += static_cast<char>(hexCode);
                            } else if ((hexCode & 0xF800) == 0)
                            {
                                result += static_cast<char>(((hexCode & 0x07C0) >> 6) | 0xC0);
                                result += static_cast<char>(((hexCode & 0x003F) >> 0) | 0x80);
                            } else
                            {
                                result += static_cast<char>(((hexCode & 0xF000) >> 12) | 0xE0);
                                result += static_cast<char>(((hexCode & 0x0FC0) >> 6) | 0x80);
                                result += static_cast<char>(((hexCode & 0x003F) >> 0) | 0x80);
                            }
                            break;
                        }
                        default:
                            return false;
                    }
                }
                else
                    result += ch;

                if (!stream.get(ch))
                    return false;
            }
            result += ch;
            term = result;
            return true;
        }
        default:
        {
            std::string result;
            while (IsValidCharacter(ch))
            {
                result += ch;
                if (!stream.get(ch))
                    break;
            }
            stream.putback(ch);
            term = result;
            return true;
        }
    }
}

Token GetToken(std::istream & stream)
{
    std::string term;
    if (!GetTerm(stream, term))
        return Token { TokenType::InvalidToken, term };
    if (utility::IsEqualIgnoreCase(term, "null"))
        return Token { TokenType::NullToken, term };
    if (utility::IsEqualIgnoreCase(term, "false"))
        return Token { TokenType::FalseToken, term };
    if (utility::IsEqualIgnoreCase(term, "true"))
        return Token { TokenType::TrueToken, term };
    if (utility::IsEqualIgnoreCase(term, "{"))
        return Token { TokenType::CurlyBraceOpen, term };
    if (utility::IsEqualIgnoreCase(term, "}"))
        return Token { TokenType::CurlyBraceClose, term };
    if (utility::IsEqualIgnoreCase(term, "["))
        return Token { TokenType::SquareBracketOpen, term };
    if (utility::IsEqualIgnoreCase(term, "]"))
        return Token { TokenType::SquareBracketClose, term };
    if (utility::IsEqualIgnoreCase(term, ","))
        return Token { TokenType::Comma, term };
    if (utility::IsEqualIgnoreCase(term, ":"))
        return Token { TokenType::Colon, term };
    if ((term.length() >= 2) && (term[0] == '"') && (term[term.length() - 1] == '"'))
    {
        return Token { TokenType::QuotedString, term.substr(1, term.length() - 2)};
    }
    if ((term.length() >= 1) && (std::isdigit(term[0]) || (term[0] == '-')))
    {
        size_t index = 0;
        if (term[index] == '-')
        {
            ++index;
            while ((index < term.length()) && (std::isspace(term[index])))
                ++index;
            if (index >= term.length())
                return Token { TokenType::InvalidToken, term };
        }
        if (!std::isdigit(term[index]))
            return Token { TokenType::InvalidToken, term };
        ++index;
        while ((index < term.length()) && (std::isdigit(term[index])))
            ++index;
        if ((index < term.length()) && std::tolower(term[index]) == '.')
        {
            ++index;
            while ((index < term.length()) && (std::isdigit(term[index])))
                ++index;
        }
        if ((index < term.length()) && std::tolower(term[index]) == 'e')
        {
            ++index;
            if ((index < term.length()) && ((term[index] == '+') || (term[index] == '-')))
                ++index;
            if (index >= term.length())
                return Token { TokenType::InvalidToken, term };
            while ((index < term.length()) && (std::isdigit(term[index])))
                ++index;
        }
        if (index < term.length())
            return Token { TokenType::InvalidToken, term };
        return Token { TokenType::Number, term };
    }
    return Token { TokenType::InvalidToken, term };
}

ValuePtr Parse(std::istream & stream)
{
    Token token = GetToken(stream);
    switch (token.type)
    {
        case TokenType::FalseToken:
            return std::make_shared<Boolean>(false);
        case TokenType::TrueToken:
            return std::make_shared<Boolean>(true);
        case TokenType::NullToken:
            return std::make_shared<Null>();
        case TokenType::QuotedString:
            return std::make_shared<String>(token.value);
        case TokenType::Number:
            return std::make_shared<Number>(token.value);
        case TokenType::CurlyBraceOpen:
        {
            stream.putback(token.value[0]);
            auto object = std::make_shared<Object>();
            if (!object->Deserialize(stream))
                return nullptr;
            return object;
        }
        case TokenType::SquareBracketOpen:
        {
            stream.putback(token.value[0]);
            auto object = std::make_shared<Array>();
            if (!object->Deserialize(stream))
                return nullptr;
            return object;
        }
        default:
            return nullptr;
    }
}

} // namespace json
