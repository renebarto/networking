#include "json/Serialization.h"

#include <sstream>
#include "json/Array.h"
#include "json/Boolean.h"
#include "json/Null.h"
#include "json/Number.h"
#include "json/Object.h"
#include "json/String.h"

namespace json {

std::string Serialize(const Array & value)
{
    std::ostringstream stream;
    value.Serialize(stream, 0);
    return stream.str();
}

std::string Serialize(const Boolean & value)
{
    std::ostringstream stream;
    value.Serialize(stream, 0);
    return stream.str();
}

std::string Serialize(const Null & value)
{
    std::ostringstream stream;
    value.Serialize(stream, 0);
    return stream.str();
}

std::string Serialize(const Number & value)
{
    std::ostringstream stream;
    value.Serialize(stream, 0);
    return stream.str();
}

std::string Serialize(const Object & value)
{
    std::ostringstream stream;
    value.Serialize(stream, 0);
    return stream.str();
}

std::string Serialize(const String & value)
{
    std::ostringstream stream;
    value.Serialize(stream, 0);
    return stream.str();
}

bool Deserialize(const std::string & text, Array & value)
{
    std::istringstream stream(text);
    return value.Deserialize(stream);
}

bool Deserialize(const std::string & text, Boolean & value)
{
    std::istringstream stream(text);
    return value.Deserialize(stream);
}

bool Deserialize(const std::string & text, Null & value)
{
    std::istringstream stream(text);
    return value.Deserialize(stream);
}

bool Deserialize(const std::string & text, Number & value)
{
    std::istringstream stream(text);
    return value.Deserialize(stream);
}

bool Deserialize(const std::string & text, Object & value)
{
    std::istringstream stream(text);
    return value.Deserialize(stream);
}

bool Deserialize(const std::string & text, String & value)
{
    std::istringstream stream(text);
    return value.Deserialize(stream);
}

} // namespace json
