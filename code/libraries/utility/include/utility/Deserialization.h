#pragma once

#include <sstream>
#include <string>
#include "utility/StringFunctions.h"

namespace serialization {

template<class T>
typename std::enable_if<!std::is_enum<T>::value, bool>::type
Deserialize(const std::string & text, T & value)
{
    T t;
    std::istringstream stream(text);
    stream >> t;
    if (!stream.bad())
    {
        value = t;
        return true;
    }
    return false;
}

// String Deserialization
bool Deserialize(const std::string & text, bool & value);
bool Deserialize(const std::string & text, int8_t & value, int base = 10);
bool Deserialize(const std::string & text, uint8_t & value, int base = 10);
bool Deserialize(const std::string & text, int16_t & value, int base = 10);
bool Deserialize(const std::string & text, uint16_t & value, int base = 10);
bool Deserialize(const std::string & text, int32_t & value, int base = 10);
bool Deserialize(const std::string & text, uint32_t & value, int base = 10);
bool Deserialize(const std::string & text, int64_t & value, int base = 10);
bool Deserialize(const std::string & text, uint64_t & value, int base = 10);
bool Deserialize(const std::string & text, float & value);
bool Deserialize(const std::string & text, double & value);

inline bool Deserialize(const std::string & text, std::string & value)
{
    value = text;
    return true;
}
bool Deserialize(const std::string & text, std::wstring & value);

template<class T>
class IStringDeserializer
{
public:
    IStringDeserializer()
    {}

    virtual ~IStringDeserializer()
    {}

    virtual bool Deserialize(const std::string & text, T & result) = 0;
};

} // namespace serialization

