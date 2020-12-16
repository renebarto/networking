#pragma once

#include "utility/Deserialization.h"
#include "utility/Serialization.h"

namespace json {

class Array;
class Boolean;
class Null;
class Number;
class Object;
class String;

template<class T>
class Deserializer : public utility::IStringDeserializer<T>
{
public:
    Deserializer()
    {}

    virtual ~Deserializer()
    {}

    virtual bool Deserialize(const std::string & text, T & value)
    {
        return json::Deserialize(text, value);
    }
};

template<class T>
class Serializer : public utility::IStringSerializer<T>
{
public:
    Serializer()
    {}

    virtual ~Serializer()
    {}

    virtual void Serialize(std::string & text, const T & value)
    {
        text = json::Serialize(value);
    }
};

bool Deserialize(const std::string & text, Array & value);
bool Deserialize(const std::string & text, Boolean & value);
bool Deserialize(const std::string & text, Null & value);
bool Deserialize(const std::string & text, Number & value);
bool Deserialize(const std::string & text, Object & value);
bool Deserialize(const std::string & text, String & value);
std::string Serialize(const Array & value);
std::string Serialize(const Boolean & value);
std::string Serialize(const Null & value);
std::string Serialize(const Number & value);
std::string Serialize(const Object & value);
std::string Serialize(const String & value);

} // namespace json
