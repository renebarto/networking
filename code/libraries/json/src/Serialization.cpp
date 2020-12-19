#include "json/Serialization.h"

#include <sstream>
#include "json/Array.h"
#include "json/Boolean.h"
#include "json/Null.h"
#include "json/Number.h"
#include "json/Object.h"
#include "json/String.h"

namespace json {

template<>
std::string Serialize(const json::Array & value)
{
    std::ostringstream stream;
    value.Serialize(stream, 0);
    return stream.str();
}

template<>
std::string Serialize(const json::Boolean & value)
{
    std::ostringstream stream;
    value.Serialize(stream, 0);
    return stream.str();
}

template<>
std::string Serialize(const json::Null & value)
{
    std::ostringstream stream;
    value.Serialize(stream, 0);
    return stream.str();
}

template<>
std::string Serialize(const json::Number & value)
{
    std::ostringstream stream;
    value.Serialize(stream, 0);
    return stream.str();
}

template<>
std::string Serialize(const json::Object & value)
{
    std::ostringstream stream;
    value.Serialize(stream, 0);
    return stream.str();
}

template<>
std::string Serialize(const json::String & value)
{
    std::ostringstream stream;
    value.Serialize(stream, 0);
    return stream.str();
}

template<>
bool Deserialize(const std::string & text, json::Array & value)
{
    std::istringstream stream(text);
    return value.Deserialize(stream);
}

template<>
bool Deserialize(const std::string & text, json::Boolean & value)
{
    std::istringstream stream(text);
    return value.Deserialize(stream);
}

template<>
bool Deserialize(const std::string & text, json::Null & value)
{
    std::istringstream stream(text);
    return value.Deserialize(stream);
}

template<>
bool Deserialize(const std::string & text, json::Number & value)
{
    std::istringstream stream(text);
    return value.Deserialize(stream);
}

template<>
bool Deserialize(const std::string & text, json::Object & value)
{
    std::istringstream stream(text);
    return value.Deserialize(stream);
}

template<>
bool Deserialize(const std::string & text, json::String & value)
{
    std::istringstream stream(text);
    return value.Deserialize(stream);
}

} // namespace json
