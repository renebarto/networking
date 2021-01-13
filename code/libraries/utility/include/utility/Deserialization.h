#pragma once

#include <sstream>
#include <string>
#include "utility/Endian.h"
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

// Binary Deserialization

bool Extract(const std::vector<std::uint8_t> & buffer, std::size_t & offset, void * data, std::size_t length);

template <typename T>
typename std::enable_if<!std::is_enum<T>::value, bool>::type
DeserializeBinary(T & value, const std::vector<std::uint8_t> & buffer, std::size_t & offset, utility::Endianness /*endianness = utility::Endianness::LittleEndian*/)
{
    return Extract(buffer, offset, &value, sizeof(value));
}

bool DeserializeBinary(bool & value, const std::vector<std::uint8_t> & buffer, std::size_t & offset, utility::Endianness endianness = utility::Endianness::LittleEndian);
bool DeserializeBinary(std::int8_t & value, const std::vector<std::uint8_t> & buffer, std::size_t & offset, utility::Endianness endianness = utility::Endianness::LittleEndian);
bool DeserializeBinary(std::uint8_t & value, const std::vector<std::uint8_t> & buffer, std::size_t & offset, utility::Endianness endianness = utility::Endianness::LittleEndian);
bool DeserializeBinary(std::int16_t & value, const std::vector<std::uint8_t> & buffer, std::size_t & offset, utility::Endianness endianness = utility::Endianness::LittleEndian);
bool DeserializeBinary(unsigned short & value, const std::vector<std::uint8_t> & buffer, std::size_t & offset, utility::Endianness endianness = utility::Endianness::LittleEndian);
bool DeserializeBinary(std::int32_t & value, const std::vector<std::uint8_t> & buffer, std::size_t & offset, utility::Endianness endianness = utility::Endianness::LittleEndian);
bool DeserializeBinary(std::uint32_t & value, const std::vector<std::uint8_t> & buffer, std::size_t & offset, utility::Endianness endianness = utility::Endianness::LittleEndian);
bool DeserializeBinary(std::int64_t & value, const std::vector<std::uint8_t> & buffer, std::size_t & offset, utility::Endianness endianness = utility::Endianness::LittleEndian);
bool DeserializeBinary(std::uint64_t & value, const std::vector<std::uint8_t> & buffer, std::size_t & offset, utility::Endianness endianness = utility::Endianness::LittleEndian);
bool DeserializeBinary(float & value, const std::vector<std::uint8_t> & buffer, std::size_t & offset, utility::Endianness endianness = utility::Endianness::LittleEndian);
bool DeserializeBinary(double & value, const std::vector<std::uint8_t> & buffer, std::size_t & offset, utility::Endianness endianness = utility::Endianness::LittleEndian);
bool DeserializeBinary(long double & value, const std::vector<std::uint8_t> & buffer, std::size_t & offset, utility::Endianness endianness = utility::Endianness::LittleEndian);
bool DeserializeBinary(std::string & value, const std::vector<std::uint8_t> & buffer, std::size_t & offset, utility::Endianness endianness = utility::Endianness::LittleEndian);
bool DeserializeBinary(std::wstring & value, const std::vector<std::uint8_t> & buffer, std::size_t & offset, utility::Endianness endianness = utility::Endianness::LittleEndian);


// bool DeserializeBinary(unsigned short & value, const std::vector<std::uint8_t> & buffer, std::size_t & offset, utility::Endianness endianness)
// {
//     std::uint16_t data {};
//     if (!DeserializeBinary(data, buffer, offset, endianness))
//         return false;
//     value = data;
//     return true;
// }

template<typename EnumType>
typename std::enable_if<std::is_enum<EnumType>::value, bool>::type
DeserializeBinary(EnumType & value, const std::vector<std::uint8_t> & buffer, std::size_t & offset, utility::Endianness endianness = utility::Endianness::LittleEndian)
{
    std::underlying_type<EnumType>::type data {};
    if (!DeserializeBinary(data, buffer, offset, endianness))
        return false;
    value = static_cast<EnumType>(data);
    return true;
}

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

