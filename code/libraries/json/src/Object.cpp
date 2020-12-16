#include "json/Object.h"
#include "Parse.h"
#include "json/Boolean.h"
#include "json/Null.h"
#include "json/Number.h"
#include "json/String.h"

namespace json
{

static constexpr size_t IndentSize = 4;

KVPair::KVPair()
    : m_key()
    , m_value()
{
}

KVPair::KVPair(const std::string & key, const ValuePtr & value)
    : m_key(key)
    , m_value(value)
{
}

bool KVPair::Deserialize(std::istream & stream)
{
    m_key = {};
    m_value = {};
    std::string key;
    Token token = GetToken(stream);
    if (token.type != TokenType::QuotedString)
        return false;
    key = token.value;

    token = GetToken(stream);
    if (token.type != TokenType::Colon)
        return false;

    ValuePtr value = Parse(stream);
    if (value == nullptr)
        return false;

    m_key = key;
    m_value = value;
    return true;
}

void KVPair::Serialize(std::ostream & stream, int indentDepth) const
{
    for (int i = 0; i < indentDepth; i++)
    {
        stream << std::string(IndentSize, ' ');
    }
    stream << "\"" << m_key << "\" : ";
    if (m_value != nullptr)
    {
        m_value->Serialize(stream, indentDepth, false);
    }
    else
        stream << "null";
}

Object::Object()
    : m_elements()
{

}

Object::Object(const std::initializer_list<KVPair> & elements)
    : m_elements()
{
    for (auto & element : elements)
    {
        AddPair(element);
    }
}

bool Object::AddPair(const KVPair & pair)
{
    auto result = m_elements.insert(std::pair<const std::string &, ValuePtr>(pair.GetKey(), pair.GetValue()));
    return result.second;
}

const ValuePtr Object::Find(const std::string & name) const
{
    for (auto & item : m_elements)
    {
        if (item.first == name)
            return item.second;
    }
    return nullptr;
}

ValuePtr Object::Find(const std::string & name)
{
    for (auto & item : m_elements)
    {
        if (item.first == name)
            return item.second;
    }
    return nullptr;
}

bool Object::Deserialize(std::istream & stream)
{
    std::string key;
    Token token = GetToken(stream);
    if (token.type != TokenType::CurlyBraceOpen)
        return false;

    bool success = true;
    bool empty = true;
    while (success)
    {
        KVPair pair;
        success = pair.Deserialize(stream);
        if (success)
        {
            if (!AddPair(pair))
                return false;
            empty = false;
        }
        else
            return empty;
        token = GetToken(stream);
        if (token.type == TokenType::CurlyBraceClose)
            return true;
        if (token.type != TokenType::Comma)
        {
            while (token.type != TokenType::InvalidToken)
            {
                token = GetToken(stream);
                if (token.type == TokenType::CurlyBraceClose)
                    break;
            }
            return false;
        }
    }

    token = GetToken(stream);
    return (token.type == TokenType::CurlyBraceClose);
}

void Object::Serialize(std::ostream & stream, int indentDepth, bool indentInitial) const
{
    if (indentInitial)
    {
        for (int i = 0; i < indentDepth; i++)
        {
            stream << std::string(IndentSize, ' ');
        }
    }
    stream << '{' << std::endl;
    ConstIterator iterator = GetIterator();
    while (!iterator.AtEnd())
    {
        (*iterator).Serialize(stream, indentDepth + 1);
        iterator++;
        if (!iterator.AtEnd())
            stream << ',';
        stream << std::endl;
    }
    for (int i = 0; i < indentDepth; i++)
    {
        stream << std::string(IndentSize, ' ');
    }
    stream << '}';
}

} // namespace json
