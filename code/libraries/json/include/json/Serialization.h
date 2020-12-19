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
bool Deserialize(const std::string & text, T & value);
template<>
bool Deserialize(const std::string & text, Array & value);
template<>
bool Deserialize(const std::string & text, Boolean & value);
template<>
bool Deserialize(const std::string & text, Null & value);
template<>
bool Deserialize(const std::string & text, Number & value);
template<>
bool Deserialize(const std::string & text, Object & value);
template<>
bool Deserialize(const std::string & text, String & value);
template<class T>
std::string Serialize(const T & value);
template<>
std::string Serialize(const Array & value);
template<>
std::string Serialize(const Boolean & value);
template<>
std::string Serialize(const Null & value);
template<>
std::string Serialize(const Number & value);
template<>
std::string Serialize(const Object & value);
template<>
std::string Serialize(const String & value);

template<class T>
class Deserializer : public serialization::IStringDeserializer<T>
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
class Serializer : public serialization::IStringSerializer<T>
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

} // namespace json
